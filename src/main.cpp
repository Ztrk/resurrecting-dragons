#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <mpi.h>
#include "packet.h"
#include "process.h"
#include "professional.h"
#include "task_generator.h"
using namespace std;

int getOptionValue(int argc, char **argv, const string &option, int defaultValue) {
    char **arg = find(argv, argv + argc, option);
    if (arg >= argv + argc - 1) {
        return defaultValue;
    }
    int value = atoi(*(arg + 1));
    if (value <= 0) {
        return defaultValue;
    }
    return value;
}

bool hasOption(int argc, char **argv, const string &option) {
    return find(argv, argv + argc, option) != argv + argc;
}

string get_help() {
    return 
    "\n"
    "-help                    This help\n"
    "-heads i                 Number of head professionalists\n"
    "-bodies i                Number of body professionalists\n"
    "-tails i                 Number of tail professionalists\n"
    "-offices i               Number of offices\n"
    "-skeletons i             Number of skeletons\n"
    "-work-time seconds       Time of work in office and resurrecting dragon\n"
    "-time-per-task seconds   Time between every task generation";
}

vector<Professional::Specialization> get_specializations(int heads_num, int body_num,
        int tail_num, int size) {

    vector<int> specs_nums = { heads_num, body_num, tail_num };
    int not_specified_num = (heads_num == 0) + (body_num == 0) + (tail_num == 0);
    int processes_lacking = size - heads_num - body_num - tail_num - 1;
    
    vector<Professional::Specialization> specializations;
    if (processes_lacking < not_specified_num) {
        return specializations;
    }

    for (size_t i = 0; i < specs_nums.size(); ++i) {
        if (specs_nums[i] == 0) {
            specs_nums[i] = processes_lacking / not_specified_num;
            processes_lacking -= specs_nums[i];
            --not_specified_num;
        }
    }

    specializations.push_back(Professional::Specialization::UNDEFINED);
    for (int i = 0; i < specs_nums[0]; ++i) {
        specializations.push_back(Professional::Specialization::HEAD);
    }
    for (int i = 0; i < specs_nums[1]; ++i) {
        specializations.push_back(Professional::Specialization::BODY);
    }
    for (int i = 0; i < specs_nums[2]; ++i) {
        specializations.push_back(Professional::Specialization::TAIL);
    }
    return specializations;
}

int main(int argc, char **argv) {
    const int HEADS_NUM = getOptionValue(argc, argv, "-heads", 0);
    const int BODY_NUM = getOptionValue(argc, argv, "-bodies", 0);
    const int TAIL_NUM = getOptionValue(argc, argv, "-tails", 0);

    const int OFFICE_NUM = getOptionValue(argc, argv, "-offices", 1);
    const int SKELETON_NUM = getOptionValue(argc, argv, "-skeletons", 1);
    const int WORK_TIME = getOptionValue(argc, argv, "-work-time", 2);

    const int TIME_PER_TASK = getOptionValue(argc, argv, "-time-per-task", 2);


    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    if (provided != MPI_THREAD_MULTIPLE) {
        cerr << "MPI does not provide enough thread support\n";
        cerr << "Requested: " << MPI_THREAD_MULTIPLE 
            << " provided: " << provided << '\n';
        cerr << "Exiting\n";
        MPI_Finalize();
        return 1;
    }

    Packet::initialize_datatype();

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (hasOption(argc, argv, "-help")) {
        if (rank == 0) {
            cout << get_help() << endl;
        }
        MPI_Finalize();
        return 0;
    }

    if (size < 4) {
        cout << "There must be at least 4 processes" << endl;
        MPI_Finalize();
        return 1;
    }

    auto specializations = get_specializations(HEADS_NUM, BODY_NUM, TAIL_NUM, size);
    if (specializations.size() != static_cast<size_t>(size)) {
        cerr << "Number of processes with specialization must be equal to number of process - 1" << endl;
        MPI_Finalize();
        return 1;
    }

    Process *process;
    if (rank == 0) {
        process = new TaskGenerator(TIME_PER_TASK);
    }
    else {
        process = new Professional(specializations, OFFICE_NUM, SKELETON_NUM, WORK_TIME);
    }

    process->execute();

    delete process;
    MPI_Finalize();

    return 0;
}
