#include <bits/stdc++.h>
using namespace std;
regex branch(R"(^.*(beq|bne|bge|blt ).*$)"); // |jal


long long hexToDecimal(string hex) {
    long long decimal = 0;
    long long base = 1; // Initializing base value to 1, this will be multiplied by 16 each time

    // cout<<hex<<endl;
    // Iterate over the hexadecimal string in reverse order
    for (int i = hex.length() - 1; i >= 2; i--) {
        // Convert hexadecimal digit to decimal value
        int digitValue;
        if (hex[i] >= '0' && hex[i] <= '9') {
            digitValue = hex[i] - '0';
        } else if (hex[i] >= 'A' && hex[i] <= 'F') {
            digitValue = hex[i] - 'A' + 10;
        } else if (hex[i] >= 'a' && hex[i] <= 'f') {
            digitValue = hex[i] - 'a' + 10;
        } else {
            // Invalid character in hexadecimal string
            // cout<<hex<<endl;
            // cerr << "Invalid hexadecimal input." << endl;
            return -1;
        }

        // Add the decimal value of the current digit to the result
        decimal += digitValue * base;

        // Move to the next position (decrease base by a power of 16)
        base *= 16;
    }

    return decimal;
}

string longLongToHex(long long num) {
    stringstream stream;
    stream << hex << num; // Convert long long to hexadecimal
    return stream.str(); // Return the hexadecimal string
}

void printMap(const map<string, string>& myMap) {
    for (const auto& pair : myMap) {
        cout << pair.first << " -> " << pair.second << endl;
    }
}



int main() {
    ifstream inputFile("sqrt_test_Lab.txt");
    map<string, pair<string, string>> branchMap;

    map<string,string> target_buffer_AlwaysTaken;
    map<string,string> target_buffer_NotTaken;
    map<string,string> target_buffer_1bit;
    map<string,string> target_buffer_2bit;

    int count = 1;

    int taken = 0;
    int not_taken = 0;

    int prev_state_1bit = 1; //START FROM TAKEN STATE
    int prev_state_2bit = 10; //START FROM WEAKLY TAKEN STATE
    //int err_one = 0;
    int branch_inst = 0;
    int hit_1bit=0,miss_1bit=0;
    int hit_2bit=0,miss_2bit=0;

    if(inputFile.is_open()) {
        string line, nextline;
        getline(inputFile, line);
        while (getline(inputFile, nextline)) {
            if(count > 2000000) break;
            // cout<<"line = "<<line<<endl;
            // cout<<"next line = "<<nextline<<endl;
            string pc ;
            if(line[0]=='c'){
                if(line.size()<21){
                    line=nextline;
                    continue;
                }
                pc = line.substr(10, 10);
            }
            else{
                if(line.size()<22){
                    line=nextline;
                    continue;
                }
                // cout<<"running"<<endl;
                pc = line.substr(11,10);
            }
            // cout<<"pc = "<<pc<<endl;
            string nextpc;
            if(nextline[0]=='c'){
                if(nextline.size()<21){
                    continue;
                }
                nextpc = nextline.substr(10, 10);
            }
            else{
                // cout<<"running"<<endl;
                if(nextline.size()<22){
                    continue;
                }
                nextpc = nextline.substr(11,10);
            }

            // cout<<"nextpc = "<<nextpc<<endl;
            long long pc_current = hexToDecimal(pc);
            // cout<<"pc = "<<pc<<" pc_int = "<<pc_current<<endl;
            long long pc_next = hexToDecimal(nextpc);
            // cout<<"pc_next = "<<nextpc<<" pcn    ext_int = "<<pc_next<<endl;

            if(pc_current==-1 || pc_next==-1){
                // cout<<"running"<<endl;
                count++;
                continue;
            }
        
            if (regex_match(line, branch)) {
                // cout<<"running"<<endl;
                // cout<<"pc = "<<pc<<" pc_int = "<<pc_current<<endl;
                // cout<<"pc_next = "<<nextpc<<" pcn    ext_int = "<<pc_next<<endl;
                stringstream label(line);
                string word;
                while(!label.eof()){
                    label >> word;
                    if (word == "+") break;
                }
                long long label_offset;
                label >> label_offset;
                long long pc_label = pc_current + label_offset;
                // cout<<pc_label<<endl;
                target_buffer_AlwaysTaken[pc]="0x"+longLongToHex(pc_label);
                target_buffer_NotTaken[pc]="0x"+longLongToHex(pc_current+4);
                if(((pc_next - pc_current != 4) && (prev_state_1bit==1))||((pc_next - pc_current == 4) && (prev_state_1bit==0))){
                    hit_1bit++;
                }
                else{ 
                    miss_1bit++;
                }
                if(prev_state_1bit==1){
                    target_buffer_1bit[pc]="0x"+longLongToHex(pc_label);
                }
                else if(prev_state_1bit==0){
                    target_buffer_1bit[pc]="0x"+longLongToHex(pc_current+4);
                }
                if(((pc_next - pc_current != 4) && ((prev_state_2bit/10)==1))||((pc_next - pc_current == 4) && ((prev_state_2bit/10)==0))) hit_2bit++;
                else miss_2bit++;
                if((prev_state_2bit/10)==1){
                    target_buffer_2bit[pc]="0x"+longLongToHex(pc_label);
                }
                else if(prev_state_2bit/10==0){
                    target_buffer_2bit[pc]="0x"+longLongToHex(pc_current+4);
                }
                //always taken and not taken.
                if(pc_next -pc_current != 4){
                    taken++;
                    if(prev_state_2bit==0) prev_state_2bit=1;
                    else if(prev_state_2bit==1) prev_state_2bit=10;
                    else prev_state_2bit=11;
                    prev_state_1bit=1;
                }  
                else if (pc_next - pc_current == 4){
                    not_taken++;
                    if(prev_state_2bit==11) prev_state_2bit=10;
                    else if(prev_state_2bit==10) prev_state_2bit=1;
                    else prev_state_2bit=0;
                    prev_state_1bit=0;
                }    
                branch_inst++;
            }
            line = nextline;
            count++;
        }
        inputFile.close();
    }
    else cerr << "Unable to open input file." << endl;
    inputFile.close();
    float always_taken_accuracy = (taken*100.0)/branch_inst;
    float always_notTaken_accuracy = (not_taken*100.0)/branch_inst;
    float accuracy_1bit = (hit_1bit*100.0)/branch_inst;
    float accuracy_2bit = (hit_2bit*100.0)/branch_inst;

    cout << always_taken_accuracy<<endl;

    cout<<"Branch Buffer Target for Always Taken "<<endl;
    //printMap(target_buffer_AlwaysTaken);
    cout<<endl<<endl;

    cout << always_notTaken_accuracy<<endl;
    cout<<"Branch Buffer Target for Not Taken "<<endl;
    //printMap(target_buffer_NotTaken);
    cout<<endl<<endl;

    cout << accuracy_1bit<<endl;
    cout<<"Branch Buffer Target for 1 bit predictor "<<endl;
    //printMap(target_buffer_1bit);
    cout<<endl<<endl;


    cout << accuracy_2bit<<endl;
    cout<<"Branch Buffer Target for 2 bit predictor "<<endl;
    //printMap(target_buffer_2bit);
    cout<<endl<<endl;
}