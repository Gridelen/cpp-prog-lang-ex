/*
[5] (*2) Convert a small C program to C++. Modify the header files to declare all functions
called and to declare the type of every argument. Where possible, replace #defines with
enum, const, constexpr, or inline. Remove extern declarations from .cpp files and if necessary
convert all function definitions to C++ function definition syntax. Replace calls of malloc()
and free() with new and delete. Remove unnecessary casts
*/

// virtual machine in c
// source is at https://github.com/felixangell/mac.git

// typeical run: prog E:\Development\cpp\edu\Cpp_PL_Stroustrup_4ed\Test\12_Funcs\Ex\5\mac\mac-improved\test.mac

#define  _CRT_SECURE_NO_WARNINGS 1 // avoid MSVC errors on fopen

/*
    A more advanced version of the VM
*/

#include <cstdio>

constexpr size_t STACK_SIZE = 256;
static int stack[STACK_SIZE];

/** Instructions */
enum Instructions {
    HLT, // 0  -- hlt              :: halts program
    PSH, // 1  -- psh val          :: pushes <val> to stack
    POP, // 2  -- pop              :: pops value from stack
    ADD, // 3  -- add              :: adds top two vals on stack
    MUL, // 4  -- mul              :: multiplies top two vals on stack
    DIV, // 5  -- div              :: divides top two vals on stack
    SUB, // 6  -- sub              :: subtracts top two vals on stack
    SLT, // 7  -- slt reg_a, reg_b :: pushes (reg_a < reg_b) to stack
    MOV, // 8  -- mov reg_a, reg_b :: movs the value in reg_a to reg_b
    SET, // 9  -- set reg, val     :: sets the reg to value
    LOG, // 10 -- log a            :: prints out a
    IF,  // 11 -- if reg val ip    :: if the register == val branch to the ip
    IFN, // 12 -- ifn reg val ip   :: if the register != val branch to the ip
    GLD, // 13 -- gld reg          :: loads a register to the stack
    GPT, // 14 -- gpt reg          :: pushes top of stack to the given register
    NOP  // 15 -- nop              :: nothing
};

/** Registers */
enum Registers {
    A, B, C, D, E, F, I, J, // GENERAL PURPOSE
    EX,                     // EXCESS
    EXA,                    // MORE EXCESS
    IP,                     // INSTRUCTION POINTER
    SP,                     // STACK POINTER
    Last
};

constexpr size_t REGISTER_SIZE = static_cast<size_t>(Last);
static int registers[REGISTER_SIZE];

// instructions array
int *instructions;

// how many instructions to do
int instruction_count = 0;

// how much space is allocated for the instructions
// 4 instructions by default
int instruction_space = 4;

/** if the program is running */
static bool running = true;

/** if the IP is assigned by jmp instructions(such as IF,IFN),it should not increase 1 any more **/
bool is_jmp = false;

/** quick ways to get SP and IP */
inline int sp()
{
    return registers[SP];
}
inline void sp(int i)
{
    registers[SP] = i;
}
inline int ip()
{
    return registers[IP];
}
inline void ip(int i)
{
    registers[IP] = i;
}

/** fetch current instruction set */
inline Instructions fetch()
{
    return static_cast<Instructions>(instructions[ip()]);
}

/** prints the stack from A to B */
void print_stack() {
    for (int i = 0; i < sp(); i++) {
        printf("0x%04d ", stack[i]);
        if ((i + 1) % 4 == 0) { printf("\n"); }
    }
    if (sp() != 0) { printf("\n"); }
}

void print_registers() {
    printf("Register Dump:\n");
    for (int i = 0; i < REGISTER_SIZE; i++) {
        printf("%04d ", registers[i]);
        if ((i + 1) % 4 == 0) { printf("\n"); }
    }
}

int find_empty_register() {
    for (int i = 0; i < REGISTER_SIZE; i++) {
        if (i != registers[EX] && i != registers[EXA]) { return i; }
    }
    return EX;
}

void eval(Instructions instr) {
    is_jmp = false;
    switch (instr) {
        case HLT: {
            running = false;
            printf("Finished Execution\n");
            // print_stack(0, 16);
            // print_registers();
            break;
        }
        case PSH: {
            sp(sp() + 1);
            ip(ip() + 1);
            stack[sp()] = instructions[ip()];
            break;
        }
        case POP: {
            sp(sp() - 1);
            break;
        }
        case ADD: {
            registers[A] = stack[sp()];
            sp(sp() - 1);

            registers[B] = stack[sp()];
            /* SP = SP - 1; */

            registers[C] = registers[B] + registers[A];

            /* SP = SP + 1; */
            stack[sp()] = registers[C];
            printf("%d + %d = %d\n", registers[B], registers[A], registers[C]);
            break;
        }
        case MUL: {
            registers[A] = stack[sp()];
            sp(sp() - 1);

            registers[B] = stack[sp()];
            /*SP = SP - 1;*/

            registers[C] = registers[B] * registers[A];

            /*SP = SP + 1;*/
            stack[sp()] = registers[C];
            printf("%d * %d = %d\n", registers[B], registers[A], registers[C]);
            break;
        }
        case DIV: {
            registers[A] = stack[sp()];
            sp(sp() - 1);

            registers[B] = stack[sp()];
            /* SP = SP - 1;*/

            registers[C] = registers[B] / registers[A];

            /* SP = SP + 1; */
            stack[sp()] = registers[C];
            printf("%d / %d = %d\n", registers[B], registers[A], registers[C]);
            break;
        }
        case SUB: {
            registers[A] = stack[sp()];
            sp(sp() - 1);

            registers[B] = stack[sp()];
            /* SP = SP - 1; */

            registers[C] = registers[B] - registers[A];

            /* SP = SP + 1; */
            stack[sp()] = registers[C];
            printf("%d - %d = %d\n", registers[B], registers[A], registers[C]);
            break;
        }
        case SLT: {
            sp(sp() - 1);
            stack[sp()] = stack[sp() + 1] < stack[sp()];
            break;
        }
        case MOV: {
            registers[instructions[ip() + 2]] = registers[instructions[ip() + 1]];
            ip(ip() + 2);
            break;
        }
        case SET: {
            registers[instructions[ip() + 1]] = instructions[ip() + 2];
            ip(ip() + 2);
            break;
        }
        case LOG: {
            printf("%d\n", registers[instructions[ip() + 1]]);
            ip(ip() + 1);
            break;
        }
        case IF: {
            if (registers[instructions[ip() + 1]] == instructions[ip() + 2]) {
                ip(instructions[ip() + 3]);
                is_jmp = true;
            }
            else {
                ip(ip() + 3);
            }
            break;
        }
        case IFN: {
            if (registers[instructions[ip() + 1]] != instructions[ip() + 2]) {
                ip(instructions[ip() + 3]);
                is_jmp = true;
            }
            else {
                ip(ip() + 3);
            }
            break;
        }
        case GLD: {
            sp(sp() + 1);
            ip(ip() + 1);
            stack[sp()] = registers[instructions[ip()]];
            break;
        }
        case GPT: {
            registers[instructions[ip() + 1]] = stack[sp()];
            ip(ip() + 1);
            break;
        }
        case NOP: {
            printf("Do Nothing\n");
            break;
        }
        default: {
            printf("Unknown Instruction %d\n", instr);
            break;
        }
    }
}

int main(int argc, char** argv) {
    if (argc <= 1) {
        printf("error: no input files\n");
        return -1;
    }

    // filename
    char *filename = argv[1];

    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("error: could not read file `%s`\n", filename);
        return -1;
    }

    // allocate space for instructions
    instructions = new int[instruction_space]; // 4 instructions

    // read the "binary" file
    int num;
    int i = 0;
    while (fscanf(file, "%d", &num) > 0) {
        instructions[i] = num;
        printf("%d\n", instructions[i]);
        i++;
        if (i >= instruction_space) {
            instruction_space *= 2; // double size
            delete instructions; // realloc
            instructions = new int[instruction_space];
        }
    }

    // set 'instruction_count' to number of instructions read
    instruction_count = i;

    // close the file
    fclose(file);

    // initialize stack pointer
    sp(-1);

    // loop through program, but don't
    // go out of the programs bounds
    while (running && ip() < instruction_count) {
        eval(fetch());
        if (!is_jmp) {
            ip(ip() + 1);
        }
    }

    // clean up instructions
    delete instructions;

    return 0;
}
