#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

using namespace std;

int find_tag(unsigned char*& mem, unsigned int& class_id, bool& prim, unsigned int& tag){
    unsigned char* start = mem;
    unsigned char c = mem[0];
    c = c>>6;  //right shift 6 bits to get only 1st 2 bits
    class_id = c;
    c = mem[0];
    c = c>>5;  //right shift 5 bits to get 1st 3 bits
    c = c&1;   //AND with 0..001, thus picking only the last bit
    if((unsigned int)c==1){
        prim=false;
    }
    else{
        prim=true;
    }
    c = mem[0];
    c = c&31; //AND with 00..11111, thus picking the last 5 bits
    tag = (int)c;
    if(tag==31){ //tag is in the subsequent bytes
        ++mem;
        unsigned char firstbit = mem[0]>>7;
        tag=mem[0]&127;    //zero the first bit
        while(firstbit!=0){
            mem++;
            firstbit = mem[0]>>7;
            unsigned char last7 = mem[0]&127;    //zero the first bit
            tag = tag<<7;
            tag = tag|last7;
        }
    }
    ++mem;
    return mem-start;
}

int find_length(unsigned char*& mem, unsigned int &len){
    unsigned char* start = mem;
    len=(unsigned int)mem[0];
    if(len<128){
        ++mem;
    }
    else{
        unsigned char n = mem[0]&127; //zero the first bit
        ++mem;
        if(n>sizeof(len)){
            std::cout << "holy molly! we need a bigger int\n";
        }
        len = mem[0];
        for(int i=1; i<n; i++){
            len = len<<8;
            len = len | mem[i]; //push the next byte in len
        }
        mem+=n;
    }
    return mem-start; //return the number of consumed bytes
}

uint64_t getTLV(unsigned char*& memblock, uint64_t size, string parent){ //TODO: size should be uint_64t?
    uint64_t consumed=0;
    unsigned char* start = memblock;
    unsigned int class_id, tag, len;
    bool primitive;
    while(consumed<size){
        if(consumed != (memblock-start)){
            cout << "Hola! Hola! consumed = "<< consumed <<
                ", but start="<<hex<<start << "and memblock="<<memblock<<endl;
            return -1;
        }
        consumed+= find_tag(memblock, class_id, primitive, tag);
        consumed+= find_length(memblock, len);

        //concat parent and tag
        string tag_str;
        ostringstream ss;
        ss << tag;
        if(parent.empty())
            tag_str = ss.str();
        else
            tag_str = parent+"."+ss.str();

        cout << "(Tag='"<<tag_str<< dec << "' Len='"<<len<<"' ";
        if(primitive){
            cout << "Val='";
            for(unsigned int i=0; i<len; i++){
                cout << hex <<setw(2)<<setfill('0') << (unsigned int)memblock[0]; 
                memblock++;consumed++;
            }
            cout << "')\n" ;
        }
        else{
            consumed+=getTLV(memblock,len, tag_str);
            cout << ")" ;
        }
    }
    return consumed;
}

int main(int argc, char* argv[]){

    if(argc<2){
        cerr << "BER filename must be passed as param\n";
        return 1;
    }

    string ber_file_name(argv[1]);

    //open the file with cursor at end (ate) to get the size
    ifstream in(ber_file_name.c_str(),ios::binary|ios::ate);
    const streampos size = in.tellg();
    in.close();
    if(size==-1){
        cerr << "FATAL ERROR: Failed to get the file size\n";
        return 1;
    }

    cout << "size =" << size << endl;
    ifstream file(ber_file_name.c_str(), ios::in|ios::binary);
    if(file.is_open()){
        unsigned char* memblock = new unsigned char[size]();
        file.read((char*)memblock, size);
        getTLV(memblock,size, "");
        file.close();
    }
    else {
        cout << "unable to open file: " << ber_file_name << endl;
        return 1;
    }
}
