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

// constant quarks
Quark Q_LINUX;
Quark Q_THREADS;
Quark Q_CPUS;
Quark Q_CUR_CPU;
Quark Q_CUR_THREAD;
Quark Q_RESOURCES;
Quark Q_IRQS;
Quark Q_SOFT_IRQS;
Quark Q_SYSCALL;
Quark Q_STATUS;
Quark Q_PPID;
Quark Q_EXEC_NAME;
Quark Q_IDLE;
Quark Q_RUN_USERMODE;
Quark Q_RUN_SYSCALL;
Quark Q_IRQ;
Quark Q_SOFT_IRQ;
Quark Q_UNKNOWN;
Quark Q_WAIT_BLOCKED;
Quark Q_INTERRUPTED;
Quark Q_WAIT_FOR_CPU;
Quark Q_RAISED;
Quark Q_SYS_CLONE;

// 0 to 65535 integers
Quark Q_INT[65536];

Quark getIntQ(const CurrentState& state, std::int64_t x)
{
    // optimization for pretty much all integers used in this provider
    if (x >= 0 && x < 65536) {
        return Q_INT[x];
    }

    return state.getQuark(std::to_string(x));
}

const UintEventValue& getEventCpu(const Event& event)
{
    assert(event.getStreamPacketContext());

    return event.getStreamPacketContext()["cpu_id"].asUintValue();
}

Quark getEventCpuQuark(const CurrentState& state, const Event& event)
{
    const auto& cpu = getEventCpu(event);

    return getIntQ(state, cpu.asUint());
}

std::int32_t asSint32(const SintEventValue& event)
{
    return static_cast<std::int32_t>(event.getValue());
}

std::uint32_t asUint32(const UintEventValue& event)
{
    return static_cast<std::uint32_t>(event.getValue());
}

StateNode& getLinuxNode(CurrentState& state)
{
    return state.getRoot()[Q_LINUX];
}

StateNode& getCurrentCpuNode(CurrentState& state, const Event& event)
{
    auto qCpu = getEventCpuQuark(state, event);

    return getLinuxNode(state)[Q_CPUS][qCpu];
}

StateNode& getCpuCurrentThreadNode(CurrentState& state, const Event& event)
{
    return getCurrentCpuNode(state, event)[Q_CUR_THREAD];
}

StateNode& getThreadsCurrentThreadNode(CurrentState& state, const Event& event)
{
    auto& cpuCurrentThreadNode = getCpuCurrentThreadNode(state, event);

    if (!cpuCurrentThreadNode) {
        return state.getRoot();
    }

    auto qCurrentThread = getIntQ(state, cpuCurrentThreadNode.asSint32());

    return getLinuxNode(state)[Q_THREADS][qCurrentThread];
}

StateNode& getCurrentIrqNode(CurrentState& state, const Event& event)
{
    auto& irq = event["irq"];
    auto qIrq = getIntQ(state, irq.asSint());

    return getLinuxNode(state)[Q_RESOURCES][Q_IRQS][qIrq];
}

StateNode& getCurrentSoftIrqNode(CurrentState& state, const Event& event)
{
    auto& vec = event["vec"];
    auto qVec = getIntQ(state, vec.asUint());

    return getLinuxNode(state)[Q_RESOURCES][Q_SOFT_IRQS][qVec];
}

bool onExitSyscall(CurrentState& state, const Event& event)
{
    auto& root = state.getRoot();
    auto& currentThreadNode = getThreadsCurrentThreadNode(state, event);
    auto& currentCpuNode = getCurrentCpuNode(state, event);

    if (currentThreadNode != root) {
        // reset current thread's syscall
        currentThreadNode[Q_SYSCALL].setNull();

        // current thread's status
        currentThreadNode[Q_STATUS] = Q_RUN_USERMODE;
    }

    // current CPU status
    currentCpuNode[Q_STATUS] = Q_RUN_USERMODE;

    return true;
}

bool onIrqHandlerEntry(CurrentState& state, const Event& event)
{
    auto& root = state.getRoot();
    auto& currentThreadNode = getThreadsCurrentThreadNode(state, event);
    auto& currentCpuNode = getCurrentCpuNode(state, event);
    auto& currentIrqNode = getCurrentIrqNode(state, event);
    auto& cpu = getEventCpu(event);

    // current IRQ's CPU
    currentIrqNode[Q_CUR_CPU] = asUint32(cpu);

    if (currentThreadNode != root) {
        // current thread's status
        currentThreadNode[Q_STATUS] = Q_INTERRUPTED;
    }

    // current CPU's status
    currentCpuNode[Q_STATUS] = Q_IRQ;

    return true;
}

bool onIrqHandlerExit(CurrentState& state, const Event& event)
{
    auto& root = state.getRoot();
    auto& currentThreadNode = getThreadsCurrentThreadNode(state, event);
    auto& currentCpuNode = getCurrentCpuNode(state, event);
    auto& cpuCurrentThreadNode = getCpuCurrentThreadNode(state, event);
    auto& currentIrqNode = getCurrentIrqNode(state, event);

    // reset current IRQ's CPU
    currentIrqNode[Q_CUR_CPU].setNull();

    if (currentThreadNode != root) {
        if (!currentThreadNode[Q_SYSCALL]) {
            // syscall not set for current thread: running in usermode
            currentThreadNode[Q_STATUS] = Q_RUN_USERMODE;
            currentCpuNode[Q_STATUS] = Q_RUN_USERMODE;
        } else {
            // syscall set for current thread: running a syscall
            currentThreadNode[Q_STATUS] = Q_RUN_SYSCALL;
            currentCpuNode[Q_STATUS] = Q_RUN_SYSCALL;
        }
    }

    if (!cpuCurrentThreadNode) {
        // no current thread for this CPU: CPU is idle
        currentCpuNode[Q_STATUS] = Q_IDLE;
    } else if (cpuCurrentThreadNode.asSint32() == 0) {
        currentCpuNode[Q_STATUS] = Q_IDLE;
    }

    return true;
}

bool onSoftIrqEntry(CurrentState& state, const Event& event)
{
    auto& root = state.getRoot();
    auto& currentThreadNode = getThreadsCurrentThreadNode(state, event);
    auto& currentCpuNode = getCurrentCpuNode(state, event);
    auto& currentSoftIrqNode = getCurrentSoftIrqNode(state, event);
    auto& cpu = getEventCpu(event);

    // current soft IRQ's CPU
    currentSoftIrqNode[Q_CUR_CPU] = asUint32(cpu);

    // reset current soft IRQ's status
    currentSoftIrqNode[Q_STATUS].setNull();

    if (currentThreadNode != root) {
        // current thread's status
        currentThreadNode[Q_STATUS] = Q_INTERRUPTED;
    }

    // current CPU's status
    currentCpuNode[Q_STATUS] = Q_SOFT_IRQ;

    return true;
}

bool onSoftIrqExit(CurrentState& state, const Event& event)
{
    auto& root = state.getRoot();
    auto& currentThreadNode = getThreadsCurrentThreadNode(state, event);
    auto& currentCpuNode = getCurrentCpuNode(state, event);
    auto& cpuCurrentThreadNode = getCpuCurrentThreadNode(state, event);
    auto& currentSoftIrqNode = getCurrentSoftIrqNode(state, event);

    // reset current soft IRQ's CPU
    currentSoftIrqNode[Q_CUR_CPU].setNull();

    // reset current soft IRQ's status
    currentSoftIrqNode[Q_STATUS].setNull();

    if (currentThreadNode != root) {
        if (!currentThreadNode[Q_SYSCALL]) {
            // syscall not set for current thread: running in usermode
            currentThreadNode[Q_STATUS] = Q_RUN_USERMODE;
            currentCpuNode[Q_STATUS] = Q_RUN_USERMODE;
        } else {
            // syscall set for current thread: running a syscall
            currentThreadNode[Q_STATUS] = Q_RUN_SYSCALL;
            currentCpuNode[Q_STATUS] = Q_RUN_SYSCALL;
        }
    }

    if (!cpuCurrentThreadNode) {
        // no current thread for this CPU: CPU is idle
        currentCpuNode[Q_STATUS] = Q_IDLE;
    } else if (cpuCurrentThreadNode.asSint32() == 0) {
        currentCpuNode[Q_STATUS] = Q_IDLE;
    }

    return true;
}

bool onSoftIrqRaise(CurrentState& state, const Event& event)
{
    auto& currentSoftIrqNode = getCurrentSoftIrqNode(state, event);

    // current soft IRQ's status: raised
    currentSoftIrqNode[Q_STATUS] = Q_RAISED;

    return true;
}

bool onSchedSwitch(CurrentState& state, const Event& event)
{
    auto& root = state.getRoot();
    auto& linuxNode = getLinuxNode(state);
    auto& prevState = event["prev_state"];
    auto& prevTid = event["prev_tid"];
    auto qPrevTid = getIntQ(state, prevTid.asSint());
    auto& nextTid = event["next_tid"].asSintValue();
    auto& nextComm = event["next_comm"];
    auto& currentCpuNode = getCurrentCpuNode(state, event);
    auto& threadsPrevTidStatusNode = linuxNode[Q_THREADS][qPrevTid][Q_STATUS];

    if (prevState.asSint() == 0) {
        threadsPrevTidStatusNode = Q_WAIT_FOR_CPU;
    } else {
        threadsPrevTidStatusNode = Q_WAIT_BLOCKED;
    }

    auto& newCurrentThread = linuxNode[Q_THREADS][nextTid];

    // new current thread's run mode
    if (!newCurrentThread[Q_SYSCALL]) {
        newCurrentThread[Q_STATUS] = Q_RUN_USERMODE;
    } else {
        newCurrentThread[Q_STATUS] = Q_RUN_SYSCALL;
    }

    // thread's exec name
    newCurrentThread[Q_EXEC_NAME] = nextComm.asArray().getString();

    // current CPU's current thread
    currentCpuNode[Q_CUR_THREAD] = asSint32(nextTid);

    // current CPU's status
    if (nextTid != 0L) {
        if (newCurrentThread[Q_SYSCALL]) {
            currentCpuNode[Q_STATUS] = Q_RUN_SYSCALL;
        } else {
            currentCpuNode[Q_STATUS] = Q_RUN_USERMODE;
        }
    } else {
        currentCpuNode[Q_STATUS] = Q_IDLE;
    }

    return true;
}

bool onSchedProcessFork(CurrentState& state, const Event& event)
{
    auto& linuxNode = getLinuxNode(state);
    auto& childTid = event["child_tid"];
    auto qChildTid = getIntQ(state, childTid.asSint());
    auto& parentTid = event["parent_tid"].asSintValue();
    auto qParentTid = getIntQ(state, parentTid.asSint());
    auto& childComm = event["child_comm"].asArray();
    auto& threadsChildTidNode = linuxNode[Q_THREADS][qChildTid];

    // child thread's parent TID
    threadsChildTidNode[Q_PPID] = asSint32(parentTid);

    // child thread's exec name
    threadsChildTidNode[Q_EXEC_NAME] = childComm.getString();

    // child thread's status
    threadsChildTidNode[Q_STATUS] = Q_WAIT_FOR_CPU;

    // child thread's syscall
    threadsChildTidNode[Q_SYSCALL] = linuxNode[Q_THREADS][qParentTid][Q_SYSCALL];

    if (!threadsChildTidNode[Q_SYSCALL]) {
        threadsChildTidNode[Q_SYSCALL] = Q_SYS_CLONE;
    }

    return true;
}

bool onSchedProcessFree(CurrentState& state, const Event& event)
{
    auto& linuxNode = getLinuxNode(state);
    auto qTid = getIntQ(state, event["tid"].asSint());

    // nullify thread subtree
    linuxNode[Q_THREADS][qTid].setNullRecursive();

    return true;
}

bool onLttngStatedumpProcessState(CurrentState& state, const Event& event)
{
    auto& linuxNode = getLinuxNode(state);
    auto qTid = getIntQ(state, event["tid"].asSint());
    auto& ppid = event["ppid"].asSintValue();
    auto& status = event["status"].asSintValue();
    auto& name = event["name"].asArray();
    auto& threadsTidNode = linuxNode[Q_THREADS][qTid];
    auto& threadsTidExecNameNode = threadsTidNode[Q_EXEC_NAME];
    auto& threadsTidPpidNode = threadsTidNode[Q_PPID];
    auto& threadsTidStatusNode = threadsTidNode[Q_STATUS];

    // initialize thread's exec name
    if (!threadsTidExecNameNode) {
        threadsTidExecNameNode = name.getString();
    }

    // initialize thread's parent TID
    if (!threadsTidPpidNode) {
        threadsTidPpidNode = asSint32(ppid);
    }

    // initialize thread's status
    if (!threadsTidStatusNode) {
        if (status == 2L) {
            threadsTidStatusNode = Q_WAIT_FOR_CPU;
        } else if (status == 5L) {
            threadsTidStatusNode = Q_WAIT_BLOCKED;
        } else {
            threadsTidStatusNode = Q_UNKNOWN;
        }
    }

    return true;
}

bool onSchedWakeupEvent(CurrentState& state, const Event& event)
{
    auto& linuxNode = getLinuxNode(state);
    auto qTid = getIntQ(state, event["tid"].asSint());
    auto& threadsTidStatusNode = linuxNode[Q_THREADS][qTid][Q_STATUS];

    if (threadsTidStatusNode.isQuark()) {
        if (threadsTidStatusNode.asQuark() != Q_RUN_USERMODE &&
                threadsTidStatusNode.asQuark() != Q_RUN_SYSCALL) {
            threadsTidStatusNode = Q_WAIT_FOR_CPU;
        }
    } else {
        // TODO: is this right?
        threadsTidStatusNode = Q_WAIT_FOR_CPU;
    }

    return true;
}

bool onSysEvent(CurrentState& state, const Event& event)
{
    auto& root = state.getRoot();
    auto& currentThreadNode = getThreadsCurrentThreadNode(state, event);
    auto& currentCpuNode = getCurrentCpuNode(state, event);

    if (currentThreadNode != root) {
        currentThreadNode[Q_SYSCALL] = event.getName();
        currentThreadNode[Q_STATUS] = Q_RUN_SYSCALL;
    }

    currentCpuNode[Q_STATUS] = Q_RUN_SYSCALL;

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
    adapter.registerEventCallbackRegex("^lttng-kernel$", "^compat_sys_", onSysEvent);
}

void getConstantQuarks(CurrentState& state)
{
    Q_LINUX = state.getQuark("linux");
    Q_THREADS = state.getQuark("threads");
    Q_CPUS = state.getQuark("cpus");
    Q_CUR_CPU = state.getQuark("cur-cpu");
    Q_CUR_THREAD = state.getQuark("cur-thread");
    Q_RESOURCES = state.getQuark("resources");
    Q_IRQS = state.getQuark("irqs");
    Q_SOFT_IRQS = state.getQuark("soft-irqs");
    Q_SYSCALL = state.getQuark("syscall");
    Q_STATUS = state.getQuark("status");
    Q_PPID = state.getQuark("ppid");
    Q_EXEC_NAME = state.getQuark("exec-name");
    Q_IDLE = state.getQuark("idle");
    Q_RUN_USERMODE = state.getQuark("usermode");
    Q_RUN_SYSCALL = state.getQuark("syscall");
    Q_IRQ = state.getQuark("irq");
    Q_SOFT_IRQ = state.getQuark("soft-irq");
    Q_UNKNOWN = state.getQuark("unknown");
    Q_WAIT_BLOCKED = state.getQuark("wait-blocked");
    Q_INTERRUPTED = state.getQuark("interrupted");
    Q_WAIT_FOR_CPU = state.getQuark("wait-for-cpu");
    Q_RAISED = state.getQuark("raised");
    Q_SYS_CLONE = state.getQuark("sys_clone");

    // 0 to 65535 integers
    for (int x = 0; x < 65536; ++x) {
        Q_INT[x] = state.getQuark(std::to_string(x));
    }
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
