#include <stdio.h>
#include <math.h>
#include "Datafetcher.h"
 //kan behöva samla in en del samples för att se när signalen börjar och slutar men det blir samma princip. 

 //Hitta den som är störst av de två signalera för att veta hur ska ska subtrahera. 

// en sample så får jag in en 1 eller 0 från alla tre signaler. Sen måste jag själv bygga upp tre vekoterer av detta. Sample är def i datafetcher.h

int find_index(int* signal, int length){ //behöver nog inte längden, ingår i vektorn.
    for (int i=0; i < length; i++){
        if (signal[i]==1){
            return i;
        }
    }
}

int index1 = find_index(singal1, length);
int index2 = find_index(singal2, length);

int d_t = index1-index2; 