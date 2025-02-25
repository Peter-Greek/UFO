// The MIT License (MIT)
//
// Copyright (c) 2025 Peter Greek
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Proper permission is grated by the copyright holder.
//
// Credit is attributed to the copyright holder in some form in the product.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#ifndef CSCI437_EVENTMANAGER_H
#define CSCI437_EVENTMANAGER_H

#include <iostream>
#include <map>
#include <string>
#include <memory>
#include <vector>
#include <algorithm>
#include <functional>
#include <tuple>
#include <nlohmann/json.hpp>
#include "Util.h"
#include <list>

using json = nlohmann::json;

class EventManager {
public:
    explicit EventManager(std::function<void(const std::string&, const json&)> func)
            : pgx_onTriggerEvent(std::move(func)) {}

    ~EventManager() = default;

    // Function wrapper for any callable with variadic args
    template <typename Func>
    void AddEventHandler(const std::string& eventName, Func&& func) {
        using FunctionType = std::decay_t<Func>;
        using ArgsTuple = typename function_traits<FunctionType>::args_tuple;

        eventHandlers[eventName].push_back([func = std::forward<Func>(func)](const json& eventData) {
            callWithJson(func, eventData, ArgsTuple{});
        });
    }

    template <typename... Args>
    void TriggerEvent(const std::string& eventName, Args&&... args) {
        json eventData = packArguments(std::forward<Args>(args)...);
        pgx_onTriggerEvent(eventName, eventData);
    }

    void onTriggerEvent(const std::string& eventName, const json& eventData) {
        auto it = eventHandlers.find(eventName);
        if (it != eventHandlers.end()) {
            for (auto& handler : it->second) {
                handler(eventData);
            }
        }
    }

private:
    std::map<std::string, std::list<std::function<void(json)>> > eventHandlers;
    std::function<void(const std::string&, const json&)> pgx_onTriggerEvent;

    // Converts variadic arguments into a JSON array
    template <typename... Args>
    json packArguments(Args&&... args) {
        return json::array({std::forward<Args>(args)...});
    }

    // **Better function_traits implementation**
    template <typename T>
    struct function_traits : function_traits<decltype(&T::operator())> {};

    // Specialization for function pointers
    template <typename Ret, typename... Args>
    struct function_traits<Ret (*)(Args...)> {
        using args_tuple = std::tuple<Args...>;
    };

    // Specialization for lambdas and member function pointers
    template <typename ClassType, typename Ret, typename... Args>
    struct function_traits<Ret (ClassType::*)(Args...) const> {
        using args_tuple = std::tuple<Args...>;
    };

    // Calls the function with JSON arguments by unpacking them dynamically
    template <typename Func, typename Tuple, std::size_t... Indices>
    static void invokeWithTuple(Func&& func, const json& eventData, Tuple, std::index_sequence<Indices...>) {
        func(eventData[Indices].get<std::tuple_element_t<Indices, Tuple>>()...);
    }

    template <typename Func, typename Tuple>
    static void callWithJson(Func&& func, const json& eventData, Tuple) {
        invokeWithTuple(func, eventData, Tuple{}, std::make_index_sequence<std::tuple_size_v<Tuple>>{});
    }
};

#endif //CSCI437_EVENTMANAGER_H
