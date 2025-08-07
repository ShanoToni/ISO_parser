#include <fstream>
#include <iostream>
#include <vector>

#ifndef INPUT_FILE_PATH
#define INPUT_FILE_PATH "transcript.txt"
#endif

enum class FrameType {
    single_frame = 0,
    first_frame,
    consecutive_frame,
    flow_control_frame,
    unset_frame
};

struct ParsedFrame {
    std::string id_           = "";
    FrameType   type_         = FrameType::unset_frame;
    int         size_of_data_ = 0;
    std::string data_         = "";
};

int setFrameType(ParsedFrame& in_out_frame, const std::string& type) {
    // determine type
    switch (type[0]) {
    case '0':
        in_out_frame.type_ = FrameType::single_frame;
        break;
    case '1':
        // First frame
        in_out_frame.type_ = FrameType::first_frame;
        break;
    case '2':
        in_out_frame.type_ = FrameType::consecutive_frame;
        break;
    case '3':
        in_out_frame.type_ = FrameType::flow_control_frame;
        break;
    default:
        std::cerr << "Unexpected frame type encountered : " << type[0]
                  << " expected value[0, 1, 2, 3]\n";
        return 1;
        break;
    }
    return 0;
}

int setSingleFrameData(ParsedFrame& frame, const std::string& line) {
    int size =
        std::stoi(line.substr(4, 1)) * 2; // 1 hex val = 2 chars in string
    if (size > 14) {
        std::cerr << " Unexpected size for single frame data: " << size
                  << " \n";
        return 1;
    }
    frame.data_ = line.substr(5, size);
    return 0;
}

bool controlFlowCheck(std::ifstream& file) {
    std::string line;
    if (std::getline(file, line)) {
        std::string type = line.substr(3, 2);
        if (type == "30") {
            return true;
        } else {
            std::cerr << " Unexpected type of next frame, Expected 30 got: "
                      << type << "\n";
            return false;
        }
    } else {
        std::cerr << " Unexpected EOF! \n";
        return false;
    }
}

int accumulateMultiFrameData(std::string& data, std::ifstream& file,
                             const int size) {
    std::string line;
    int         remaining_size = size;
    int         frame_num      = 1;
    while (remaining_size > 0) {
        if (std::getline(file, line)) {
            std::string type = line.substr(3, 2);
            if (type[0] == '2') {
                int line_frame_num = type[1] - '0';
                if (line_frame_num == frame_num) {
                    // each frame has max 14 chars
                    data += line.substr(5, std::min(remaining_size, 14));
                    remaining_size = remaining_size -
                                     14; // if < 0 loop terminates no more reads
                    ++frame_num;
                } else {
                    std::cerr << " Unexpected next frame sequence, Expected "
                              << frame_num << " got: " << line_frame_num
                              << "\n";
                    return 1;
                }
            } else {
                std::cerr << " Unexpected type of next frame, Expected 2 got: "
                          << type[0] << "\n";
                return 1;
            }
        } else {
            std::cerr << " Unexpected EOF! \n";
            return 1;
        }
    }
    return 0;
}

int setFirstFrameData(ParsedFrame& frame, const std::string& line,
                      std::ifstream& file) {
    int         size       = std::stoi(line.substr(5, 2), nullptr, 16) * 2;
    std::string first_data = line.substr(7, 12);

    // next frame needs to be control flow
    if (controlFlowCheck(file)) {
        int status = accumulateMultiFrameData(first_data, file, size - 12);
        frame.data_ = first_data;
        return status;
    } else {
        std::cerr << " First frame encountered without following control flow "
                     "frame! \n";
        return 1;
    }
}

int handleFrameTypes(ParsedFrame& frame, const std::string& line,
                     std::ifstream& file, std::vector<ParsedFrame>& frames) {

    int result = 0;
    switch (frame.type_) {
    case FrameType::single_frame:
        result += setSingleFrameData(frame, line);
        frames.push_back(frame);
        break;
    case FrameType::first_frame:
        result += setFirstFrameData(frame, line, file);
        frames.push_back(frame);
        break;
    default:
        std::cerr << " Unexpected frame type encountered :"
                  << static_cast<int>(frame.type_) << " \n";
        return 1;
        break;
    }
    return result;
}

int parseLine(const std::string& line, std::ifstream& file,
              std::vector<ParsedFrame>& frames) {
    // 3 chars for ID, 2 for frame type, 14 for data
    if (line.size() != 19) {
        std::cerr << "Encountered line with unexpected length: " << line.size()
                  << ", expected 19 ." << "\n";
        return 1;
    }

    std::string id   = line.substr(0, 3);
    std::string type = line.substr(3, 2);

    ParsedFrame frame;
    frame.id_  = id;
    int result = setFrameType(frame, type);
    result += handleFrameTypes(frame, line, file, frames);

    return result;
}

int parseInput(std::vector<ParsedFrame>& frames) {
    std::ifstream file(INPUT_FILE_PATH);
    if (!file.is_open()) {
        std::cerr << "Failed to open file at : " << INPUT_FILE_PATH << "\n";
        return 1;
    }

    std::cout << "Reading from file: " << INPUT_FILE_PATH << "\n";
    std::string line;
    int         status = 0;
    while (std::getline(file, line)) {
        status = parseLine(line, file, frames);
        if (status != 0) {
            break;
        }
    }
    return status;
}

int main() {
    std::vector<ParsedFrame> frames;
    int                      result = parseInput(frames);
    if (result != 0) {
        std::cout << "Failed to parse! \n";
    }
    for (auto frame : frames) {
        std::cout << frame.id_ << ": " << frame.data_ << "\n";
    }
    return 0;
}
