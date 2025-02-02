/*
 The MIT License (MIT)

 Copyright (c) 2017 Daniel Illescas Romero <https://github.com/illescasDaniel>

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#pragma once

#include <fstream>
#include <string>
#include <stdexcept>
#include <memory>
#include <iostream>

#if (__cplusplus >= 201406)
#include <experimental/optional>
#endif

namespace evt {

	class Log {

	public:
		enum Mode {
			exception,
			errorOutput,
			normalOutput
		};
	private:
#if (__cplusplus >= 201406)
		static constexpr Mode errorGlobalMode = Mode::errorOutput;
#endif
	public:
#if (__cplusplus >= 201406)
		static void message(const char* message) {
			if constexpr (Log::errorGlobalMode == Mode::exception) {
				throw std::runtime_error(message);
			}
			else if constexpr (Log::errorGlobalMode == Mode::errorOutput) {
				std::cerr << "Error: " << message << std::endl;
			}
			else if constexpr (Log::normalOutput == Mode::normalOutput) {
				std::cout << "Error: " << message << std::endl;
			}
		}
#endif
		static void message(const char* message, Mode errorMode = Mode::normalOutput) {
			if (errorMode == Mode::exception) {
				throw std::runtime_error(message);
			}
			else if (errorMode == Mode::errorOutput) {
				std::cerr << "Error: " << message << std::endl;
			}
			else if (errorMode == Mode::normalOutput) {
				std::cout << "Error: " << message << std::endl;
			}
		}
	};
	//
	class File {

	public:

		enum Mode {
			binary,
			normal,
			both
		};

		enum Encoding {
			NONE,
			UTF8,
		};

	private:

		std::fstream fileStream;
		std::ios_base::openmode inputOutputMode = 0;
		std::string fileName_;
		Mode mode{ Mode::both };

		void open(const std::ios_base::openmode inputOutputMode) {

			if (this->inputOutputMode != inputOutputMode) {

				close();
				this->inputOutputMode = inputOutputMode;
				fileStream.open(fileName_, inputOutputMode);
				if (fileStream.fail()) {
					Log::message("File couldn't be open");
				}
			}
		}

		void incompatibleMode() {
			Log::message("Incompatible Mode");
		}

	public:

		File(const std::string& fileName, const Mode mode = Mode::both) {
			this->fileName_ = fileName;
			this->mode = mode;
		}

		static File* openInBinary(const std::string& fileName) {
			return new File(fileName, Mode::binary);
		}

		static File* openPlainText(const std::string& fileName) {
			return new File(fileName, Mode::normal);
		}

		static bool exists(const std::string& filePath) {
			std::ifstream file(filePath);
			return !file.fail();
		}

		/* BINARY */

#if (__cplusplus >= 201406)
		template <typename Type>
		void writeInBinary(Type&& content, bool appendContent = true) {

			if (mode == Mode::normal) { incompatibleMode(); return; }

			if (appendContent) {
				open(std::ios::binary | std::ios::out | std::ios::in | std::ios::app);
			}
			else { open(std::ios::binary | std::ios::out | std::ios::in | std::ios::trunc); }

			if constexpr (!std::is_same<Type, std::string>()) {
				fileStream.write(reinterpret_cast<char*>(&content), sizeof(content));
			}
			else {
				fileStream.write(content.c_str(), content.length());
			}
		}
#else
		template <typename Type, typename = typename std::enable_if<std::is_same<Type, std::string>::value>::type>
		void writeInBinary(const Type& text, bool appendContent = true) {

			if (mode == Mode::normal) { incompatibleMode(); return; }

			if (appendContent) {
				open(std::ios::binary | std::ios::out | std::ios::in | std::ios::app);
			}
			else { open(std::ios::binary | std::ios::out | std::ios::in | std::ios::trunc); }
			fileStream.write(text.c_str(), text.length());
		}

		template <typename Type, typename = typename std::enable_if<!std::is_same<Type, std::string>::value>::type>
		void writeInBinary(Type contentToWrite, bool appendContent = true) {

			if (mode == Mode::normal) { incompatibleMode(); return; }

			if (appendContent) {
				open(std::ios::binary | std::ios::out | std::ios::in | std::ios::app);
			}
			else { open(std::ios::binary | std::ios::out | std::ios::in | std::ios::trunc); }
			fileStream.write(reinterpret_cast<char*>(&contentToWrite), sizeof(contentToWrite));
		}
#endif

		template <typename Type, typename = typename std::enable_if<!std::is_same<Type, std::string>::value>::type>
		Type readFromBinary() {

			if (mode == Mode::normal) { incompatibleMode(); return Type{}; }

			Type readInput{};
			open(std::ios::in | std::ios::binary);
			fileStream.read(reinterpret_cast<char*>(&readInput), sizeof(readInput));
			return readInput;
		}

		template <typename Type, typename = typename std::enable_if<std::is_same<Type, std::string>::value>::type>
		Type readFromBinary(std::size_t size) {

			if (mode == Mode::normal) { incompatibleMode(); return Type{}; }

			open(std::ios::in | std::ios::binary);

			std::unique_ptr<char> readTextChar(new char[size]);
			fileStream.read(readTextChar.get(), size);

			return readTextChar.get();
		}

		void seekPosition(std::size_t offsetPosition, std::ios_base::seekdir position = std::ios::beg) {

			if (mode == Mode::normal) { incompatibleMode(); return; }

			fileStream.seekp(offsetPosition, position);
		}

		/* TEXT */

		template <typename Type>
		File& write(const Type& contentToWrite, bool appendContent = true) {

			if (mode == Mode::binary) { incompatibleMode(); return *this; }

			if (appendContent) { open(std::ios::out | std::ios::in | std::ios::app); }
			else { open(std::ios::out | std::ios::in | std::ios::trunc); }

			fileStream << contentToWrite;
			return *this;
		}

		template <typename Type>
		File& writeUTF8(const Type& contentToWrite, bool appendContent = true) {

			if (mode == Mode::binary) { incompatibleMode(); return *this; }

			if (appendContent) { open(std::ios::out | std::ios::in | std::ios::app); }
			else {
				open(std::ios::out | std::ios::in | std::ios::trunc);

				unsigned char smarker[] = { 0xEF, 0xBB, 0xBF };
				fileStream << smarker;
			}

			fileStream << contentToWrite;
			return *this;
		}

		// Reads text content word by word (?)
		std::string read() {

			if (mode == Mode::binary) { incompatibleMode(); return std::string{}; }

			std::string readContent;
			open(std::ios::in);
			fileStream >> readContent;

			return readContent;
		}

		std::string getline() {

			if (mode == Mode::binary) { incompatibleMode(); return std::string{}; }

			std::string s;

			open(std::ios::in);
			std::getline(fileStream, s);

			return s;
		}

#if (__cplusplus >= 201406)

		std::experimental::optional<std::string> safeRead() {

			if (mode == Mode::binary) { incompatibleMode(); return std::string{}; }

			std::string readContent;
			open(std::ios::in);
			fileStream >> readContent;

			if ((fileStream.eof() && readContent.empty()) || fileStream.fail()) {
				return std::experimental::nullopt;
			}

			return readContent;
		}

		std::experimental::optional<std::string> safeGetline() {

			if (mode == Mode::binary) { incompatibleMode(); return std::string{}; }

			std::string s;

			open(std::ios::in);
			std::getline(fileStream, s);

			if ((fileStream.eof() && s.empty()) || fileStream.fail()) {
				return std::experimental::nullopt;
			}

			return s;
		}

#endif

		std::string toString() {

			this->open(std::ios::in);

			std::string line;
			std::string outputContent;

			while (std::getline(fileStream, line)) {
				outputContent += line + '\n';
			}

			return outputContent;
		}

		static std::string toString(const std::string& fileName) {
			return File(fileName).toString();
		}

		static void saveTextTo(const std::string& fileName, const std::string& text) {
			File fileToWrite(fileName);
			fileToWrite.open(std::ios::out);
			fileToWrite.write(text);
		}

		/* OTHER */

		void seekInputPosition(std::size_t offsetPosition, std::ios_base::seekdir position = std::ios::beg) {
			fileStream.seekg(offsetPosition, position);
		}

		void open(const std::string& fileName, const Mode mode = Mode::both) {
			this->fileName_ = fileName;
			this->mode = mode;
			this->close();
		}

		bool endOfFile() const {
			return fileStream.eof();
		}

		std::string fileName() const noexcept {
			return fileName_;
		}

		// Optional to use, the class automatically closes the file
		void close() {
			if (fileStream.is_open()) {
				fileStream.close();
			}
		}

		~File() {
			close();
		}
	};
}
