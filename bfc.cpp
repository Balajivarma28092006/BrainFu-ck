#include <cstdint>
#include <fstream>
#include <iostream>
#include <map>
#include <stack>
#include <vector>

// fucntion to pre parse the code and map the jump table
std::map<size_t, size_t> compute_jump_table(const std::string &code) {
  std::map<size_t, size_t> jump_table;
  std::stack<size_t> st;

  for (size_t i = 0; i < code.length(); ++i) {
    if (code[i] == '[') {
      st.push(i);
    } else if (code[i] == ']') {
      if (st.empty()) {
        std::cerr << "Error unmatched ']' at position " << i << std::endl;
        exit(1);
      }
      size_t start = st.top();
      st.pop();
      jump_table[start] = i; // map the ['s index to ]'s
      jump_table[i] = start; // vice versa
    }
  }

  if (!st.empty()) {
    std::cerr << "Error: Unmatched '[' at position " << st.top() << std::endl;
    exit(1);
  }
  return jump_table;
}

void interpret(const std::string &code) {
  // 30,000 cells are initialized to 0 thats what mainly used with all as 8 bit
  // unsigned integers
  std::vector<uint8_t> tape(30000, 0);
  size_t dp = 0; // data pointer
  size_t ip = 0; // instruction pointer

  std::map<size_t, size_t> jump_table = compute_jump_table(code);

  while (ip < code.length()) {
    char cmd = code[ip];

    switch (cmd) {
    case '>':
      // move to the cell after to the current
      if (dp == tape.size() - 1) {
        dp = 0;
      } else {
        dp++;
      }
      break;
    case '<':
      // move to the cell before the current one
      if (dp == 0) {
        dp = tape.size() - 1;
      } else {
        dp--;
      }
      break;
    case '.':
      // print the char
      std::cout << static_cast<char>(tape[dp]) << std::flush;
      break;
    case '+':
      // increase the current cell value
      tape[dp]++;
      break;
    case '-':
      // decerease the current cell value
      tape[dp]--;
      break;
    case '[':
      // so if the current cell has a value of 0 the instruction is terminated
      // and jumps to the nearest ]
      if (tape[dp] == 0) {
        ip = jump_table[ip];
      }
      break;
    case ']':
      // only move to the nearest [ is the current cell value is not 0
      if (tape[dp] != 0) {
        ip = jump_table[ip] - 1;
      }
      break;
    case ',':
      // for taking a char input and storing in the current cell
      char input_char;
      while (std::cin.get(input_char) &&
             (input_char == '\n' || input_char == '\r'))
        ;
      if (std::cin) {
        tape[dp] = static_cast<uint8_t>(input_char);
      } else {
        tape[dp] = 0;
      }
      break;
    default:
      // ignore any other characters
      break;
    }
    ip++;
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <filename.bf>" << std::endl;
    return 1;
  }

  std::ifstream file(argv[1]);
  if (!file.is_open()) {
    std::cerr << "Error: Could not open file " << argv[1] << std::endl;
    return 1;
  }

  std::string code((std::istreambuf_iterator<char>(file)),
                   std::istreambuf_iterator<char>());
  file.close();
  interpret(code);
  return 0;
}
