/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Peter Greek
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * Proper permission is grated by the copyright holder.
 *
 * Credit is attributed to the copyright holder in some form in the product.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

//
// Created by xerxe on 3/6/2025.
//

#ifndef CSCI437_JSONLOADER_H
#define CSCI437_JSONLOADER_H

#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <utility>
using json = nlohmann::json;

class jsonLoader {
private:
    std::string jsonPath;
    nlohmann::json jsonData;
public:
    explicit jsonLoader(std::string path) : jsonPath(std::move(path)) {
        std::ifstream i(jsonPath);
        i >> jsonData;
    }

    // add a default constructor
    jsonLoader() = default;

    nlohmann::json& get() {
        return jsonData;
    }

    void set(nlohmann::json data) {
        jsonData = std::move(data);
    }

    void save() {
        std::ofstream o(jsonPath);
        o << std::setw(4) << jsonData << std::endl;
    }

    void update() {
        std::ifstream i(jsonPath);
        i >> jsonData;
    }

    // Overload operator[] for getting/setting values
    json& operator[](const std::string& key) {
        return jsonData[key];
    }
};
using jLoader = jsonLoader;

#endif //CSCI437_JSONLOADER_H
