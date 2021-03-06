#include "BatchEnc.h"

#include <algorithm>

BatsEncoder* BatsEncoder_new(int M, int K, int T, SymbolType *input){ return new BatsEncoder(M,K,T,input); }

SymbolType* BatsEncoder_genPacket(BatsEncoder* encoder){ return encoder->genPacket();}
// Patch to expose the interface for degree distribution. Tom Lam on 11 July, 2012.
void BatsEncoder_setDegreeDist(BatsEncoder* encoder, double* degreeDist, int maxDeg){ encoder->setDegreeDist(degreeDist, maxDeg); }
void BatsEncoder_selectDegree(BatsEncoder* encoder){ encoder->selectDegree(); }

int BatsEncoder_getSmallestBid(BatsEncoder* encoder) 
{ 
	return encoder->getSmallestBid();
}

void BatsEncoder::genCheckPkg(){
	precode->GenerateCheckPackets(*buf);
}

void BatsEncoder::setInputPackets(SymbolType *input){
    buf->SetBuffer(input);
    genCheckPkg(); 
}

void BatsEncoder::genBatchWithKey(SymbolType **batch, KeyType key){
    int i;

    int Tout = packetSize + (batchSize * fieldOrder / SYMBOLSIZE);//need to know header len
	
    for (i = 0; i < batchSize; i++) {
		//reset packet buffer
        memset(batch[i], 0 , Tout);
		//set header for coding vector
		curPacket->SetForWriteWOKey(batch[i]);
		curPacket->SetHeader(i);
		//save location
		payload[i] = curPacket->GetPayload();
	}
    
	builder->Build(key);
}

