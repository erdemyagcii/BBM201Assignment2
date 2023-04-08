#include <iostream>
#include <sstream>
#include <fstream>
using namespace std;

void split(string writing, char dlm, string *ptr){
    istringstream ss(writing);
    string del1;
    int i = 0;
    while(getline(ss, del1, dlm)) {
        *(ptr + i) = del1;
        i++;
    }
}

struct flat{
    int id;
    int initial_bandwidth;
    bool is_empty;
    struct flat* next;
    struct flat* prev;
};

struct apartment{
    string name;
    int max_bandwidth;
    struct apartment* next = nullptr;
    struct flat* firstFlat = nullptr;
};

typedef struct flat flat;
typedef struct apartment apartment;

apartment* searchAfter(apartment* phead, string name) {
    apartment* pstartingNode = phead;
    while (true) {
        if(pstartingNode->name == name)
            return pstartingNode;
        else {
            pstartingNode = pstartingNode->next;
        }
    }
}

apartment* searchBefore(apartment* head, string name) {
    apartment* pstartingNode = head;
    while (true) {
        if(pstartingNode->next->name == name)
            return pstartingNode;
        else {
            pstartingNode = pstartingNode->next;
        }
    }
}


int main(int argc, char** argv) {
    string inputFilePath = argv[1];
    ifstream readInput(inputFilePath);
    string line;
    apartment* phead = nullptr;
    ofstream  dosyaYaz(argv[2]);


    while(getline(readInput, line)) {
        string arr[5];
        istringstream ss(line);
        string del;
        int i = 0;
        while (getline(ss, del, '\t')) {
            arr[i] = del;
            i++;
        }

        string fsomb = arr[0];
        string fsomb2 = arr[3];


        if (arr[0] == "add_apartment") {
            apartment* pNewApartment = new apartment;
            int max_bandwidth = atoi(arr[3].c_str());
            pNewApartment->name = arr[1];
            pNewApartment->max_bandwidth = max_bandwidth;
            pNewApartment->firstFlat = nullptr;
            if(arr[2] == "head") {
                phead = pNewApartment;
                phead->next = phead;
            }
            else {
                string *ptr = new string;
                split(arr[2], '_', ptr);
                if(*ptr == "after") {
                    apartment* temp = searchAfter(phead, *(ptr + 1));
                    pNewApartment->next = temp->next;
                    temp->next = pNewApartment;
                }
                else{
                    apartment* temp = searchBefore(phead, *(ptr + 1));
                    pNewApartment->next = temp->next;
                    temp->next = pNewApartment;
                }
            }
        }


        else if(arr[0] == "add_flat"){
            int index = atoi(arr[2].c_str());
            int initial_bandwidth = atoi(arr[3].c_str());
            int flat_id = atoi(arr[4].c_str());
            string apartment_name = arr[1];
            apartment* apartment = searchAfter(phead, apartment_name);
            flat* newFlat = new flat;
            if(apartment->firstFlat == nullptr){
                if(initial_bandwidth > apartment->max_bandwidth)
                    newFlat->initial_bandwidth = apartment->max_bandwidth;
                else
                    newFlat->initial_bandwidth = initial_bandwidth;
                if(newFlat->initial_bandwidth == 0)
                    newFlat->is_empty = true;
                newFlat->id = flat_id;
                newFlat->next = nullptr;
                newFlat->prev = nullptr;
                apartment->firstFlat = newFlat;
            }
            else{
                int totalBandwidth = 0;
                flat* startFlat1 = apartment->firstFlat;
                flat* startFlat2 = apartment->firstFlat;
                while(startFlat1->next != nullptr){
                    totalBandwidth += startFlat1->initial_bandwidth;
                    startFlat1 = startFlat1->next;
                }
                totalBandwidth += startFlat1->initial_bandwidth;

                if(index - 1 < 0){
                    if(apartment->max_bandwidth - totalBandwidth >= initial_bandwidth)
                        newFlat->initial_bandwidth = initial_bandwidth;
                    else
                        newFlat->initial_bandwidth = apartment->max_bandwidth - totalBandwidth;
                    if(newFlat->initial_bandwidth == 0)
                        newFlat->is_empty = true;
                    newFlat->id = flat_id;
                    apartment->firstFlat->prev = newFlat;
                    newFlat->next = apartment->firstFlat;
                    newFlat->prev = nullptr;
                    apartment->firstFlat = newFlat;
                }
                else {
                    int startIndex = 0;
                    while (startIndex != index - 1) {
                        startFlat2 = startFlat2->next;
                        startIndex++;
                    }
                    if (apartment->max_bandwidth - totalBandwidth >= initial_bandwidth)
                        newFlat->initial_bandwidth = initial_bandwidth;
                    else
                        newFlat->initial_bandwidth = apartment->max_bandwidth - totalBandwidth;
                    if (newFlat->initial_bandwidth == 0)
                        newFlat->is_empty = true;
                    newFlat->id = flat_id;
                    if (startFlat2->next == nullptr) {
                        startFlat2->next = newFlat;
                        newFlat->prev = startFlat2;
                        newFlat->next = nullptr;
                    } else {
                        startFlat2->next->prev = newFlat;
                        newFlat->next = startFlat2->next;
                        startFlat2->next = newFlat;
                        newFlat->prev = startFlat2;
                    }
                }
            }
        }

        else if(arr[0] == "remove_apartment"){
            string apartment_name = arr[1];
            apartment_name.erase(apartment_name.size() - 1);
            apartment* apartment1 = searchAfter(phead, apartment_name);
            if(apartment1->firstFlat == nullptr)
                delete apartment1->firstFlat;
            else if( apartment1->firstFlat->next == nullptr)
                delete apartment1->firstFlat;
            else {
                flat *tempFlat = apartment1->firstFlat->next;
                while (true) {
                    delete apartment1->firstFlat;
                    apartment1->firstFlat = tempFlat;
                    tempFlat = tempFlat->next;
                    if (apartment1->firstFlat == nullptr)
                        break;
                }
            }
            apartment* prevApartment = phead;
            while(prevApartment->next->name != apartment_name){
                prevApartment = prevApartment->next;
            }
            prevApartment->next = apartment1->next;
            delete apartment1;
        }

        else if(arr[0] == "merge_two_apartments"){//secondu firste ekle
            string firstApartmentName = arr[1];
            string secondApartmentName = arr[2];
            secondApartmentName.erase(secondApartmentName.size() - 1);
            apartment* firstApartment = searchAfter(phead,firstApartmentName);
            apartment* secondApartment = searchAfter(phead,secondApartmentName);


            if(secondApartment->firstFlat == nullptr){
                apartment* apartment = phead;
                while(apartment->next->name != secondApartment->name){
                    apartment = apartment->next;
                }
                apartment->next = secondApartment->next;
                firstApartment->max_bandwidth += secondApartment->max_bandwidth;
                delete secondApartment;
            }
            else if(firstApartment->firstFlat == nullptr){
                firstApartment->firstFlat = secondApartment->firstFlat;
                secondApartment->firstFlat = nullptr;

                apartment* apartment = phead;
                while(apartment->next->name != secondApartment->name){
                    apartment = apartment->next;
                }
                apartment->next = secondApartment->next;
                firstApartment->max_bandwidth += secondApartment->max_bandwidth;
                delete secondApartment;
            }
            else{
                flat* flat = firstApartment->firstFlat;
                while(flat->next != nullptr)
                    flat = flat->next;
                flat->next = secondApartment->firstFlat;
                secondApartment->firstFlat->prev = flat;
                apartment* apartment = phead;
                while(apartment->next->name != secondApartment->name){
                    apartment = apartment->next;
                }
                apartment->next = secondApartment->next;
                firstApartment->max_bandwidth += secondApartment->max_bandwidth;
                delete secondApartment;
            }
        }


        else if((fsomb.erase(fsomb.size() - 1)) == "find_sum_of_max_bandwidths"){
            int sum = phead->max_bandwidth;
            apartment* temp = phead->next;
            while(temp != phead){
                sum += temp->max_bandwidth;
                temp = temp->next;
            }
            dosyaYaz<<"sum of bandwwidth: "<<sum<<endl;
        }


        else if(arr[0] == "list_apartments"){
            apartment* apartment = phead;
            while(true){
                dosyaYaz<<apartment->name<<"("<<apartment->max_bandwidth<<")"<<"\t";
                if(apartment->next == phead)
                    break;
                flat* flat = apartment->firstFlat;
                while(true){
                    dosyaYaz<<"Flat"<<flat->id<<" ("<<flat->initial_bandwidth<<")"<<"\t";
                    if(flat->next == nullptr) {
                        dosyaYaz << endl;
                        break;
                    }
                    flat = flat->next;
                }
                apartment = apartment->next;
            }
        }


        else if(arr[0] == "make_flat_empty"){
            int flat_id = atoi(arr[2].c_str());
            string apartment_name = arr[1];
            apartment* apartment = searchAfter(phead, apartment_name);
            flat* startFlat = apartment->firstFlat;
            while(startFlat->id != flat_id)
                startFlat = startFlat->next;
            startFlat->is_empty = true;
            startFlat->initial_bandwidth = 0;
        }

        else if(arr[0] == "relocate_flats_to_same_apartment"){//this function not working correctly everytime so I kept this part in comment block
            dosyaYaz<<"relocate_flats_to_same_aparment function not working"<<endl;
            /*int flat_id_to_shift = atoi(arr[2].c_str());
            string* ptr1 = new string ;
            string* ptr2 = new string ;
            string* ptr3 = new string ;
            fsomb2 = fsomb2.erase((fsomb2.size() - 1));
            split(fsomb2, '[', ptr1);
            split(*(ptr1 + 1), ']', ptr2);
            int lenghtOfFlatIdList =  ((*ptr2).length()) - (((*ptr2).length())/2);
            split(*ptr2, ',', ptr3);
            int* flat_id_list = new int;
            for(int i = 0; i < lenghtOfFlatIdList; i++){
                int element = stoi(*(ptr3 + i));
                *(flat_id_list + i) = element;
            }
            string apartment_name = arr[1];
            apartment* newApartment = searchAfter(phead, apartment_name);
            flat* flatToShift = newApartment->firstFlat;
            while(flatToShift->id != flat_id_to_shift && flatToShift->next->id != flat_id_to_shift)
                flatToShift = flatToShift->next;
            for(int i = 0; i < lenghtOfFlatIdList; i++){
                int id  = *(flat_id_list + i);
                apartment* apartment = phead;
                while(true){
                    if(apartment->next == phead)
                        break;
                    flat* flat1 = apartment->firstFlat;
                    flat* flatNext = flat1->next;
                    flat* flatPrev = flat1->prev;
                    while(true){
                            flatToShift->prev->next = flat1;
                            flat1->prev = flatToShift->prev;
                            flatToShift->prev = flat1;
                            flat1->next = flatToShift;
                            flatPrev->next = flatNext;
                            flatNext->prev = flatPrev;
                            break;
                        if(flat1->next == nullptr)
                            break;
                        flat1 = flat1->next;
                    }
                    apartment = apartment->next;
                }
            }*/
        }
    }
    readInput.close();
    return 0;
}
