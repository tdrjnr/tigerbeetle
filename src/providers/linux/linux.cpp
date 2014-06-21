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
quark_t QP_LINUX;
quark_t QP_THREADS;
quark_t QP_CPUS;
quark_t QP_CUR_CPU;
quark_t QP_CUR_THREAD;
quark_t QP_RESOURCES;
quark_t QP_IRQS;
quark_t QP_SOFT_IRQS;
quark_t QP_SYSCALL;
quark_t QP_STATUS;
quark_t QP_PPID;
quark_t QP_EXEC_NAME;

// constant string value quarks
quark_t QV_IDLE;
quark_t QV_RUN_USERMODE;
quark_t QV_RUN_SYSCALL;
quark_t QV_IRQ;
quark_t QV_SOFT_IRQ;
quark_t QV_UNKNOWN;
quark_t QV_WAIT_BLOCKED;
quark_t QV_INTERRUPTED;
quark_t QV_WAIT_FOR_CPU;
quark_t QV_RAISED;
quark_t QV_SYS_CLONE;

const UintEventValue& getEventCpu(const Event& event)
{
    assert(event.getStreamPacketContext());

    return event.getStreamPacketContext()["cpu_id"].asUintValue();
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

    return root[QP_LINUX][QP_CPUS][cpu];
}

StateNode& getCpuCurrentThreadNode(StateNode& root, const Event& event)
{
    return getCurrentCpuNode(root, event)[QP_CUR_THREAD];
}

StateNode& getThreadsCurrentThreadNode(StateNode& root, const Event& event)
{
    auto& cpuCurrentThreadNode = getCpuCurrentThreadNode(root, event);

    if (cpuCurrentThreadNode == root || !cpuCurrentThreadNode) {
        return root;
    }

    return root[QP_LINUX][QP_THREADS][cpuCurrentThreadNode.asSint32Value()];
}

StateNode& getCurrentIrqNode(StateNode& root, const Event& event)
{
    auto& irq = event["irq"].asSintValue();

    return root[QP_LINUX][QP_RESOURCES][QP_IRQS][irq];
}

StateNode& getCurrentSoftIrqNode(StateNode& root, const Event& event)
{
    auto& vec = event["vec"].asUintValue();

    return root[QP_LINUX][QP_RESOURCES][QP_SOFT_IRQS][vec];
}

bool onExitSyscall(CurrentState& state, const Event& event)
{
    auto& root = state.getRoot();
    auto& currentThreadNode = getThreadsCurrentThreadNode(root, event);
    auto& currentCpuNode = getCurrentCpuNode(root, event);

    if (currentThreadNode != root) {
        // reset current thread's syscall
        currentThreadNode[QP_SYSCALL].setNull();

        // current thread's status
        currentThreadNode[QP_STATUS] = QV_RUN_USERMODE;
    }

    // current CPU status
    currentCpuNode[QP_STATUS] = QV_RUN_USERMODE;

    return true;
}

bool onIrqHandlerEntry(CurrentState& state, const Event& event)
{
    auto& root = state.getRoot();
    auto& currentThreadNode = getThreadsCurrentThreadNode(root, event);
    auto& currentCpuNode = getCurrentCpuNode(root, event);
    auto& currentIrqNode = getCurrentIrqNode(root, event);
    auto& cpu = getEventCpu(event);

    // current IRQ's CPU
    currentIrqNode[QP_CUR_CPU].setInt(asUint32(cpu));

    if (currentThreadNode != root) {
        // current thread's status
        currentThreadNode[QP_STATUS] = QV_INTERRUPTED;
    }

    // current CPU's status
    currentCpuNode[QP_STATUS] = QV_IRQ;

    return true;
}

bool onIrqHandlerExit(CurrentState& state, const Event& event)
{
    auto& root = state.getRoot();
    auto& currentThreadNode = getThreadsCurrentThreadNode(root, event);
    auto& currentCpuNode = getCurrentCpuNode(root, event);
    auto& currentIrqNode = getCurrentIrqNode(root, event);

    // reset current IRQ's CPU
    currentIrqNode[QP_CUR_CPU].setNull();

    if (currentThreadNode != root) {
        if (!currentThreadNode[QP_SYSCALL]) {
            // syscall not set for current thread: running in usermode
            currentThreadNode[QP_STATUS] = QV_RUN_USERMODE;
            currentCpuNode[QP_STATUS] = QV_RUN_USERMODE;
        } else {
            // syscall set for current thread: running a syscall
            currentThreadNode[QP_STATUS] = QV_RUN_SYSCALL;
            currentCpuNode[QP_STATUS] = QV_RUN_SYSCALL;
        }
    }

    if (!currentCpuNode[QP_CUR_THREAD]) {
        // no current thread for this CPU: CPU is idle
        currentCpuNode[QP_STATUS] = QV_IDLE;
    } else if (currentCpuNode[QP_CUR_THREAD].asSint32() == 0) {
        currentCpuNode[QP_STATUS] = QV_IDLE;
    }

    return true;
}

bool onSoftIrqEntry(CurrentState& state, const Event& event)
{
    auto& root = state.getRoot();
    auto& currentThreadNode = getThreadsCurrentThreadNode(root, event);
    auto& currentCpuNode = getCurrentCpuNode(root, event);
    auto& currentSoftIrqNode = getCurrentSoftIrqNode(root, event);
    auto& cpu = getEventCpu(event);

    // current soft IRQ's CPU
    currentSoftIrqNode[QP_CUR_CPU].setInt(asUint32(cpu));

    // reset current soft IRQ's status
    currentSoftIrqNode[QP_STATUS].setNull();

    if (currentThreadNode != root) {
        // current thread's status
        currentThreadNode[QP_STATUS] = QV_INTERRUPTED;
    }

    // current CPU's status
    currentCpuNode[QP_STATUS] = QV_SOFT_IRQ;

    return true;
}

bool onSoftIrqExit(CurrentState& state, const Event& event)
{
    auto& root = state.getRoot();
    auto& currentThreadNode = getThreadsCurrentThreadNode(root, event);
    auto& currentCpuNode = getCurrentCpuNode(root, event);
    auto& currentSoftIrqNode = getCurrentSoftIrqNode(root, event);

    // reset current soft IRQ's CPU
    currentSoftIrqNode[QP_CUR_CPU].setNull();

    // reset current soft IRQ's status
    currentSoftIrqNode[QP_STATUS].setNull();

    if (currentThreadNode != root) {
        if (!currentThreadNode[QP_SYSCALL]) {
            // syscall not set for current thread: running in usermode
            currentThreadNode[QP_STATUS] = QV_RUN_USERMODE;
            currentCpuNode[QP_STATUS] = QV_RUN_USERMODE;
        } else {
            // syscall set for current thread: running a syscall
            currentThreadNode[QP_STATUS] = QV_RUN_SYSCALL;
            currentCpuNode[QP_STATUS] = QV_RUN_SYSCALL;
        }
    }

    if (!currentCpuNode[QP_CUR_THREAD]) {
        // no current thread for this CPU: CPU is idle
        currentCpuNode[QP_STATUS] = QV_IDLE;
    } else if (currentCpuNode[QP_CUR_THREAD].asSint32() == 0) {
        currentCpuNode[QP_STATUS] = QV_IDLE;
    }

    return true;
}

bool onSoftIrqRaise(CurrentState& state, const Event& event)
{
    auto& root = state.getRoot();
    auto& currentSoftIrqNode = getCurrentSoftIrqNode(root, event);

    // current soft IRQ's status: raised
    currentSoftIrqNode[QP_STATUS] = QV_RAISED;

    return true;
}

bool onSchedSwitch(CurrentState& state, const Event& event)
{
    auto& root = state.getRoot();
    auto& prevState = event["prev_state"].asSintValue();
    auto& prevTid = event["prev_tid"].asSintValue();
    auto& nextTid = event["next_tid"].asSintValue();
    auto& nextComm = event["next_comm"];
    auto& currentCpuNode = getCurrentCpuNode(root, event);
    auto& threadsPrevTidStatusNode = root[QP_THREADS][prevTid][QP_STATUS];

    if (prevState.asSint() == 0) {
        threadsPrevTidStatusNode = QV_WAIT_FOR_CPU;
    } else {
        threadsPrevTidStatusNode = QV_WAIT_BLOCKED;
    }

    auto& newCurrentThread = root[QP_THREADS][nextTid];

    // new current thread's run mode
    if (!newCurrentThread[QP_SYSCALL]) {
        newCurrentThread[QP_STATUS] = QV_RUN_USERMODE;
    } else {
        newCurrentThread[QP_STATUS] = QV_RUN_SYSCALL;
    }

    // thread's exec name
    newCurrentThread[QP_EXEC_NAME] = nextComm.asArray().getString();

    // current CPU's current thread
    currentCpuNode[QP_CUR_THREAD] = nextTid;

    // current CPU's status
    if (nextTid != 0L) {
        if (newCurrentThread[QP_SYSCALL]) {
            currentCpuNode[QP_STATUS] = QV_RUN_SYSCALL;
        } else {
            currentCpuNode[QP_STATUS] = QV_RUN_USERMODE;
        }
    } else {
        currentCpuNode[QP_STATUS] = QV_IDLE;
    }

    return true;
}

bool onSchedProcessFork(CurrentState& state, const Event& event)
{
    auto& root = state.getRoot();
    auto& childTid = event["child_tid"].asSintValue();
    auto& parentTid = event["parent_tid"].asSintValue();
    auto& childComm = event["child_comm"].asArray();
    auto& threadsChildTidNode = root[QP_THREADS][childTid];

    // child thread's parent TID
    threadsChildTidNode[QP_PPID] = parentTid;

    // child thread's exec name
    threadsChildTidNode[QP_EXEC_NAME] = childComm.getString();

    // child thread's status
    threadsChildTidNode[QP_STATUS] = QV_WAIT_FOR_CPU;

    // child thread's syscall
    threadsChildTidNode[QP_SYSCALL] = root[QP_THREADS][parentTid][QP_SYSCALL];

    if (!threadsChildTidNode[QP_SYSCALL]) {
        threadsChildTidNode[QP_SYSCALL] = QV_SYS_CLONE;
    }

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

bool onSchedWakeupEvent(CurrentState& state, const Event& event)
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
    adapter.registerEventCallbackRegex("^lttng-kernel$", "^sched_wakeup", onSchedWakeupEvent);
    adapter.registerEventCallbackRegex("^lttng-kernel$", "^sys_", onSysEvent);
    adapter.registerEventCallbackRegex("^lttng-kernel$", "^compat_sys_", onCompatSysEvent);
}

void getConstantQuarks(CurrentState& state)
{
    // subpath quarks
    QP_LINUX = state.getSubpathQuark("linux");
    QP_THREADS = state.getSubpathQuark("threads");
    QP_CPUS = state.getSubpathQuark("cpus");
    QP_CUR_CPU = state.getSubpathQuark("cur-cpu");
    QP_CUR_THREAD = state.getSubpathQuark("cur-thread");
    QP_RESOURCES = state.getSubpathQuark("resources");
    QP_IRQS = state.getSubpathQuark("irqs");
    QP_SOFT_IRQS = state.getSubpathQuark("soft-irqs");
    QP_SYSCALL = state.getSubpathQuark("syscall");
    QP_STATUS = state.getSubpathQuark("status");
    QP_PPID = state.getSubpathQuark("ppid");
    QP_EXEC_NAME = state.getSubpathQuark("exec-name");

    // string value quarks
    QV_IDLE = state.getStringValueQuark("idle");
    QV_RUN_USERMODE = state.getStringValueQuark("usermode");
    QV_RUN_SYSCALL = state.getStringValueQuark("syscall");
    QV_IRQ = state.getStringValueQuark("irq");
    QV_SOFT_IRQ = state.getStringValueQuark("soft-irq");
    QV_UNKNOWN = state.getStringValueQuark("unknown");
    QV_WAIT_BLOCKED = state.getStringValueQuark("wait-blocked");
    QV_INTERRUPTED = state.getStringValueQuark("interrupted");
    QV_WAIT_FOR_CPU = state.getStringValueQuark("wait-for-cpu");
    QV_RAISED = state.getStringValueQuark("raised");
    QV_SYS_CLONE = state.getStringValueQuark("sys_clone");
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
