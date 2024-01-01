# This script compares the outputs produced by the program for the sample inputs with the sample solutions
# Place both the sample solutions and the outputs on the root folder of this script

import filecmp

output_files = ["1_output.txt", "2_output.txt", "3_output.txt","input_hex1_output.txt", "input_hex2_output.txt", "input_hex3_output.txt"]
# 
# 
solution_files = ["solution1.txt", "solution2.txt", "solution3.txt","solution_hex1.txt","solution_hex2.txt","solution_hex3.txt"]

for i in range(len(output_files)):
    print("Test {}".format(i))
    if (not filecmp.cmp(output_files[i], solution_files[i])):
        print("Correct Solution")
    else:
        print("Incorrect Solution")
