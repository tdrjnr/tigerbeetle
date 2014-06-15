/* This is a simple Linux kernel state provider which records the state
 * of processes and threads. It is based on the work of Florian Wininger,
 * reimplementing his XML state machine using tigerbeetle.
 *
 * His XML state machine was found at:
 *
 *   <http://secretaire.dorsal.polymtl.ca/~fwininger/XmlStateProvider/kernel-0.6.state-schema.xml>
 *
 * Copyright (c) 2014 Philippe Proulx <eepp.ca>
 *
 * This file is part of tigerbeetle.
 *
 * tigerbeetle is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tigerbeetle is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tigerbeetle.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <iostream>
#include <cassert>
#include <cstring>

#include <common/state/CurrentState.hpp>
#include <common/state/StateNode.hpp>
#include <common/stateprov/DynamicLibraryStateProvider.hpp>
#include <common/trace/Event.hpp>
#include <common/trace/TraceSet.hpp>

using namespace tibee;
using namespace tibee::common;

namespace
{

// constant subpath quarks
quark_t QUARK_P_THREADS;
quark_t QUARK_P_CPUS;
quark_t QUARK_P_CUR_THREAD;
quark_t QUARK_P_RESOURCES;
quark_t QUARK_P_IRQS;
quark_t QUARK_P_SOFT_IRQS;
quark_t QUARK_P_SYSCALL;
quark_t QUARK_P_STATUS;
quark_t QUARK_P_PPID;
quark_t QUARK_P_EXEC_NAME;

// constant string value quarks
quark_t QUARK_V_IDLE;
quark_t QUARK_V_RUN_USERMODE;
quark_t QUARK_V_RUN_SYSCALL;
quark_t QUARK_V_IRQ;
quark_t QUARK_V_SOFT_IRQ;
quark_t QUARK_V_UNKNOWN;
quark_t QUARK_V_WAIT_BLOCKED;
quark_t QUARK_V_INTERRUPTED;
quark_t QUARK_V_WAIT_FOR_CPU;
quark_t QUARK_V_SOFT_IRQ_RAISED;

const AbstractEventValue& getEventCpu(const Event& event)
{
    assert(event.getStreamPacketContext());

    return event.getStreamPacketContext()["cpu_id"];
}

std::int32_t asSint32(const AbstractEventValue& event)
{
    return static_cast<std::int32_t>(event.asSint());
}

std::uint32_t asUint32(const AbstractEventValue& event)
{
    return static_cast<std::uint32_t>(event.asUint());
}

StateNode& getCurrentCpuNode(StateNode& root, const Event& event)
{
    auto& cpu = getEventCpu(event);

    return root[QUARK_P_CPUS][cpu.asUintValue()];
}

StateNode& getCpuCurrentThreadNode(StateNode& root, const Event& event)
{
    return getCurrentCpuNode(root, event)[QUARK_P_CUR_THREAD];
}

StateNode& getThreadsCurrentThreadNode(StateNode& root, const Event& event)
{
    auto& cpuCurrentThreadNode = getCpuCurrentThreadNode(root, event);

    if (cpuCurrentThreadNode == root || !cpuCurrentThreadNode) {
        return root;
    }

    return root[QUARK_P_THREADS][cpuCurrentThreadNode.asSint32Value()];
}

StateNode& getCurrentIrqNode(StateNode& root, const Event& event)
{
    auto& irq = event["irq"];

    return root[QUARK_P_RESOURCES][QUARK_P_IRQS][irq.asSintValue()];
}

StateNode& getCurrentSoftIrqNode(StateNode& root, const Event& event)
{
    auto& vec = event["vec"];

    return root[QUARK_P_RESOURCES][QUARK_P_SOFT_IRQS][vec.asUintValue()];
}

bool onExitSyscall(CurrentState& state, const Event& event)
{
    auto& root = state.getRoot();
    auto& currentThreadNode = getThreadsCurrentThreadNode(root, event);
    auto& currentCpuNode = getCurrentCpuNode(root, event);

    if (currentThreadNode != root) {
        // remove current thread's syscall
        currentThreadNode[QUARK_P_SYSCALL].setNull();

        // set current thread's status
        currentThreadNode[QUARK_P_STATUS] = QUARK_V_RUN_USERMODE;
    }

    // set current CPU status
    currentCpuNode[QUARK_P_STATUS] = QUARK_V_RUN_USERMODE;

    return true;
}

bool onIrqHandlerEntry(CurrentState& state, const Event& event)
{
    auto& root = state.getRoot();
    auto& currentThreadNode = getThreadsCurrentThreadNode(root, event);
    auto& currentCpuNode = getCurrentCpuNode(root, event);
    auto& currentIrqNode = getCurrentIrqNode(root, event);
    auto& cpu = getEventCpu(event);

    // set current IRQ's CPU
    currentIrqNode.setInt(asUint32(cpu.asUintValue()));

    if (currentThreadNode != root) {
        // set current thread's status
        currentThreadNode[QUARK_P_STATUS] = QUARK_V_INTERRUPTED;
    }

    // set current CPU's status
    currentCpuNode[QUARK_P_STATUS] = QUARK_V_IRQ;

    return true;
}

bool onIrqHandlerExit(CurrentState& state, const Event& event)
{
    auto& root = state.getRoot();
    auto& currentThreadNode = getThreadsCurrentThreadNode(root, event);
    auto& currentCpuNode = getCurrentCpuNode(root, event);
    auto& currentIrqNode = getCurrentIrqNode(root, event);

    // remove current IRQ's CPU
    currentIrqNode.setNull();

    if (currentThreadNode != root) {
        if (!currentThreadNode[QUARK_P_SYSCALL]) {
            currentThreadNode[QUARK_P_STATUS] = QUARK_V_RUN_USERMODE;
            currentCpuNode[QUARK_P_STATUS] = QUARK_V_RUN_USERMODE;
        } else {
            currentThreadNode[QUARK_P_STATUS] = QUARK_V_RUN_SYSCALL;
            currentCpuNode[QUARK_P_STATUS] = QUARK_V_RUN_SYSCALL;
        }
    }

    if (!currentCpuNode[QUARK_P_CUR_THREAD]) {
        currentCpuNode[QUARK_P_STATUS] = QUARK_V_IDLE;
    } else if (currentCpuNode[QUARK_P_CUR_THREAD].asSint32() == 0) {
        currentCpuNode[QUARK_P_STATUS] = QUARK_V_IDLE;
    }

    return true;
}

bool onSoftIrqEntry(CurrentState& state, const Event& event)
{
    return true;
}

bool onSoftIrqExit(CurrentState& state, const Event& event)
{
    return true;
}

bool onSoftIrqRaise(CurrentState& state, const Event& event)
{
    return true;
}

bool onSchedSwitch(CurrentState& state, const Event& event)
{

    return true;
}

bool onSchedProcessFork(CurrentState& state, const Event& event)
{
    return true;
}

bool onSchedProcessFree(CurrentState& state, const Event& event)
{
    return true;
}

bool onLttngStatedumpProcessState(CurrentState& state, const Event& event)
{
    return true;
}

bool onSchedWakeup(CurrentState& state, const Event& event)
{
    return true;
}

bool onSysEvent(CurrentState& state, const Event& event)
{
    return true;
}

bool onCompatSysEvent(CurrentState& state, const Event& event)
{
    return true;
}

void registerSimpleEventCallback(DynamicLibraryStateProvider::Adapter& adapter,
                                 const char* name,
                                 const AbstractStateProvider::OnEventFunc& func)
{
    adapter.registerEventCallback("lttng-kernel", name, func);
}

void registerEventCallbacks(DynamicLibraryStateProvider::Adapter& adapter)
{
    registerSimpleEventCallback(adapter, "exit_syscall", onExitSyscall);
    registerSimpleEventCallback(adapter, "irq_handler_entry", onIrqHandlerEntry);
    registerSimpleEventCallback(adapter, "irq_handler_exit", onIrqHandlerExit);
    registerSimpleEventCallback(adapter, "softirq_entry", onSoftIrqEntry);
    registerSimpleEventCallback(adapter, "softirq_exit", onSoftIrqExit);
    registerSimpleEventCallback(adapter, "softirq_raise", onSoftIrqRaise);
    registerSimpleEventCallback(adapter, "sched_switch", onSchedSwitch);
    registerSimpleEventCallback(adapter, "sched_process_fork", onSchedProcessFork);
    registerSimpleEventCallback(adapter, "sched_process_free", onSchedProcessFree);
    registerSimpleEventCallback(adapter, "lttng_statedump_process_state", onLttngStatedumpProcessState);
    registerSimpleEventCallback(adapter, "sched_wakeup", onSchedWakeup);
    adapter.registerEventCallbackRegex("^lttng-kernel$", "^sys_", onSysEvent);
    adapter.registerEventCallbackRegex("^lttng-kernel$", "^compat_sys_", onCompatSysEvent);
}

void getConstantQuarks(CurrentState& state)
{
    // subpath quarks
    QUARK_P_THREADS = state.getSubpathQuark("threads");
    QUARK_P_CPUS = state.getSubpathQuark("cpus");
    QUARK_P_CUR_THREAD = state.getSubpathQuark("cur-thread");
    QUARK_P_RESOURCES = state.getSubpathQuark("resources");
    QUARK_P_IRQS = state.getSubpathQuark("irqs");
    QUARK_P_SOFT_IRQS = state.getSubpathQuark("soft-irqs");
    QUARK_P_SYSCALL = state.getSubpathQuark("syscall");
    QUARK_P_STATUS = state.getSubpathQuark("status");
    QUARK_P_PPID = state.getSubpathQuark("ppid");
    QUARK_P_EXEC_NAME = state.getSubpathQuark("exec-name");

    // string value quarks
    QUARK_V_IDLE = state.getStringValueQuark("idle");
    QUARK_V_RUN_USERMODE = state.getStringValueQuark("usermode");
    QUARK_V_RUN_SYSCALL = state.getStringValueQuark("syscall");
    QUARK_V_IRQ = state.getStringValueQuark("irq");
    QUARK_V_SOFT_IRQ = state.getStringValueQuark("soft-irq");
    QUARK_V_UNKNOWN = state.getStringValueQuark("unknown");
    QUARK_V_WAIT_BLOCKED = state.getStringValueQuark("wait-blocked");
    QUARK_V_INTERRUPTED = state.getStringValueQuark("interrupted");
    QUARK_V_WAIT_FOR_CPU = state.getStringValueQuark("wait-for-cpu");
    QUARK_V_SOFT_IRQ_RAISED = state.getStringValueQuark("soft-irq-raised");
}

}

extern "C" void onInit(CurrentState& state,
                       const TraceSet* traceSet,
                       DynamicLibraryStateProvider::Adapter& adapter)
{
    auto& config = adapter.getConfig();

    std::cout << "hello from linux.so: onInit()" << std::endl;
    std::cout << "  my name is: \"" << config.getName() << "\"" << std::endl;
    std::cout << "  my instance name is: \"" << config.getInstanceName() << "\"" << std::endl;
    std::cout << "  and here are my parameters:" << std::endl;

    for (const auto& keyValuePair : config.getParams()) {
        std::cout << "    " << keyValuePair.first << " = " << keyValuePair.second << std::endl;
    }

    // register events callbacks
    registerEventCallbacks(adapter);

    // get a few known quarks
    getConstantQuarks(state);

    // get indexes of interesting event fields
    // TODO
}
