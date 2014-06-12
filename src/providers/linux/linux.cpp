#include <iostream>
#include <cassert>
#include <cstring>

#include <common/state/CurrentState.hpp>
#include <common/stateprov/DynamicLibraryStateProvider.hpp>
#include <common/trace/Event.hpp>
#include <common/trace/TraceSet.hpp>

using namespace tibee;
using namespace tibee::common;

namespace
{

void printEventDetails(const Event& event)
{
    auto fields = event.getFields();

    if (fields) {
        std::cout << "<" << event.getName() << "> " << "(" <<
                     event.getTraceId() <<
                     ", " <<
                     event.getId() <<
                     ")  " << fields->toString() <<
                     std::endl;
    }
}

bool onSchedSwitch(CurrentState& state, const Event& event)
{
    assert(std::strcmp(event.getName(), "sched_switch") == 0);

    std::cout << "sched switching!" << std::endl;

    printEventDetails(event);

    return true;
}

bool onSysOpen(CurrentState& state, const Event& event)
{
    assert(std::strcmp(event.getName(), "sys_open") == 0);

    std::cout << "sys opening!" << std::endl;

    assert(event["hello"].isNull());
    assert(event["filename"].isString());
    assert(event["flags"].isSint());
    assert(event["mode"].isUint());

    std::cout << "===> filename: " << event["filename"].asString() << std::endl;
    std::cout << "===> flags:    " << event["flags"].asSint() << std::endl;
    std::cout << "===> mode:     " << event["mode"].asUint() << std::endl;

    printEventDetails(event);

    return true;
}

bool onSysClose(CurrentState& state, const Event& event)
{
    assert(std::strcmp(event.getName(), "sys_close") == 0);

    std::cout << "sys closing!" << std::endl;

    printEventDetails(event);

    return true;
}

bool onEvent(CurrentState& state, const Event& event)
{
    assert(std::strcmp(event.getName(), "sched_switch") != 0);
    assert(std::strcmp(event.getName(), "sys_close") != 0);
    assert(std::strcmp(event.getName(), "sys_open") != 0);

    std::cout << "##### OTHER EVENT #####" << std::endl;

    printEventDetails(event);

    return true;
}

bool onSysEvent(CurrentState& state, const Event& event)
{
    assert(event.getName()[0] == 's' &&
           event.getName()[1] == 'y' &&
           event.getName()[2] == 's');

    std::cout << ">>>>> syscall <<<<<" << std::endl;

    printEventDetails(event);

    return true;
}

}

extern "C" void onInit(CurrentState& state,
                       const TraceSet* traceSet,
                       DynamicLibraryStateProvider::StateProviderConfig& config)
{
    std::cout << "hello from linux.so: onInit()" << std::endl;

    if (config.registerEventCallback("lttng-kernel", "sched_switch", onSchedSwitch)) {
        std::cout << "successfully registered callback for lttng-kernel/sched_switch" << std::endl;
    } else {
        std::cout << "could not register callback for lttng-kernel/sched_switch" << std::endl;
    }

    if (config.registerEventCallback("lttng-kernel", "sys_open", onSysOpen)) {
        std::cout << "successfully registered callback for lttng-kernel/sys_open" << std::endl;
    } else {
        std::cout << "could not register callback for lttng-kernel/sys_open" << std::endl;
    }

    if (config.registerEventCallback("lttng-kernel", "sys_close", onSysClose)) {
        std::cout << "successfully registered callback for lttng-kernel/sys_close" << std::endl;
    } else {
        std::cout << "could not register callback for lttng-kernel/sys_close" << std::endl;
    }

    if (config.registerEventCallback("lttng-kernel", "meowmix", onSysClose)) {
        std::cout << "successfully registered callback for lttng-kernel/meowmix" << std::endl;
    } else {
        std::cout << "could not register callback for lttng-kernel/meowmix" << std::endl;
    }

    if (config.registerEventCallbackRegex("lttng-(?:kernel|ust)$", "^sys_(\\w|\\d)+$", onSysEvent)) {
        std::cout << "successfully registered callback for lttng-(?:kernel|ust)$/^sys_(\\w|\\d)+$" << std::endl;
    } else {
        std::cout << "could not register callback for lttng-(?:kernel|ust)$/^sys_\\w+$" << std::endl;
    }

    if (config.registerEventCallback("lttng-kernel", "", onEvent)) {
        std::cout << "successfully registered callback for lttng-kernel/*" << std::endl;
    } else {
        std::cout << "could not register callback for lttng-kernel/*" << std::endl;
    }

    const auto& tracesInfos = traceSet->getTracesInfos();

    std::cout << "traces infos:" << std::endl;

    for (const auto& traceInfos : tracesInfos) {
        std::cout << *traceInfos << std::endl;
    }
}
