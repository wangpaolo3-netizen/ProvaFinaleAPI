#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define changed -1
#define notChanged -2

#define endlessDistance INT_MAX

#define ArrayInitialSize 1024
#define cellMaxAirRoute 5
#define noAirRoute -1

#define maxCost 100
#define minCost 0

#define maxV 10
#define minV -10

typedef struct HexagonalCell {
    int cost;
    int MinHeapIndex;

    int distance;

    int AirRouteNumber;
    int* AirRoute;
} HexagonalCell;
typedef HexagonalCell* CellPointer;

typedef CellPointer* DataStructurePointer;


CellPointer MatrixOfHexagonal=NULL;
int matrixRow=0;
int matrixColumn=0;
int cellNumber = 0;
CellPointer endOfMatrix = 0;

int queueTail=0;
int queueHead=0;

CellPointer CellNULL = NULL;

DataStructurePointer Structure=NULL;

int DataStructureSize=0;
int ArraySize = ArrayInitialSize; 

void freeMatrixOfHexagonal(){
    for(int i=0;i<cellNumber;i++){
        free(MatrixOfHexagonal[i].AirRoute);
    }
    free(MatrixOfHexagonal);
    MatrixOfHexagonal = NULL;
    matrixRow=0;
    matrixColumn=0;
}

void init(int matrixRowsNumber,int matrixColumnsNumber){
    if(MatrixOfHexagonal!=NULL){
        freeMatrixOfHexagonal();
        free(Structure);
    }
    matrixRow = matrixRowsNumber;
    matrixColumn = matrixColumnsNumber;
    cellNumber = matrixColumn*matrixRow;
    MatrixOfHexagonal = (CellPointer) malloc(sizeof(HexagonalCell) * cellNumber);
    //Structure usato sia come minHeap sia come coda
    Structure = (DataStructurePointer) malloc(sizeof(CellPointer) * ArraySize);
    if (MatrixOfHexagonal && Structure){
        endOfMatrix = MatrixOfHexagonal + cellNumber - 1;
        for(int i=0;i<cellNumber;i++){
            MatrixOfHexagonal[i].cost=1;
            MatrixOfHexagonal[i].MinHeapIndex=-50;
            MatrixOfHexagonal[i].AirRoute = NULL;
            MatrixOfHexagonal[i].AirRouteNumber = 0;
        }
        printf("OK");
    }else{
        free(MatrixOfHexagonal);
        free(Structure);
        printf("KO");
    }
    
};


void ToggleAirRoute(int startCellRow,int startCellColumn,int destinationCellRow,int destinationCellColumn){
    int startCoordinates = startCellColumn * matrixRow + startCellRow;
    int destinationCoordinates = destinationCellColumn * matrixRow + destinationCellRow;
    int AirRouteCount = MatrixOfHexagonal[startCoordinates].AirRouteNumber;
    int foundAirRoute = 0;

    for(int i=0;i<AirRouteCount;i++){
        if(MatrixOfHexagonal[startCoordinates].AirRoute[i]==destinationCoordinates){ //Rimozione della rotta aerea
            foundAirRoute = 1;
            MatrixOfHexagonal[startCoordinates].AirRoute[i]=noAirRoute;
        }
        if(foundAirRoute==1 && i<AirRouteCount-1){ //Traslazione delle rotte aeree dovuta alla rimozione
            MatrixOfHexagonal[startCoordinates].AirRoute[i]=MatrixOfHexagonal[startCoordinates].AirRoute[i+1];
        }else if(foundAirRoute==1 && i==AirRouteCount-1){//Fine traslazione e riallocazione dell'array airRoute
            MatrixOfHexagonal[startCoordinates].AirRouteNumber=MatrixOfHexagonal[startCoordinates].AirRouteNumber-1;
            MatrixOfHexagonal[startCoordinates].AirRoute = (int*)realloc(MatrixOfHexagonal[startCoordinates].AirRoute,sizeof(int)*(AirRouteCount-1));
            printf("OK");
            return;
        }
    }
    if(foundAirRoute==0 && AirRouteCount<5){//Aggiunta di una rotta aerea 
        MatrixOfHexagonal[startCoordinates].AirRouteNumber = MatrixOfHexagonal[startCoordinates].AirRouteNumber+1;
        MatrixOfHexagonal[startCoordinates].AirRoute = (int*)realloc(MatrixOfHexagonal[startCoordinates].AirRoute,sizeof(int)*(AirRouteCount+1));
        MatrixOfHexagonal[startCoordinates].AirRoute[AirRouteCount]=destinationCoordinates;
        printf("OK");
        return;
    }
    printf("KO");
    return;
};


int HeapFather(int index){
    return (index-1)/2;
}
int HeapLeft(int index){
    return index*2+1;
}
int HeapRight(int index){
    return index*2+2;
}

void OrderElement(int index){
    CellPointer swapSupport=NULL;
    int fatherIndex=HeapFather(index);
    while(Structure[fatherIndex]->distance > Structure[index]->distance){
        swapSupport = Structure[fatherIndex];

        Structure[fatherIndex] = Structure[index];
        Structure[fatherIndex]->MinHeapIndex=fatherIndex;

        Structure[index] = swapSupport;
        Structure[index]->MinHeapIndex=index;
        
        index = fatherIndex;
        fatherIndex=HeapFather(index);
    }
}

void HeapInsert(CellPointer nodeToInsert){
    int index = DataStructureSize;
    Structure[index]=nodeToInsert;
    Structure[index]->MinHeapIndex=index;
    
    OrderElement(index);

    DataStructureSize++;
    if(DataStructureSize==ArraySize){
        ArraySize = ArraySize*2;
        Structure = (DataStructurePointer)realloc(Structure,sizeof(CellPointer)*ArraySize);
    }
}



CellPointer HeapRemove(){
    CellPointer nodeToRemove = Structure[0];
    nodeToRemove->MinHeapIndex=-1;
    
    int indexOfRemoved = 0;
    int leftOfRemoved = 1;
    int rightOfRemoved = 2;
    
    while(rightOfRemoved < DataStructureSize){
        if(Structure[rightOfRemoved]->distance<Structure[leftOfRemoved]->distance){
            Structure[indexOfRemoved] = Structure[rightOfRemoved];
            Structure[indexOfRemoved]->MinHeapIndex=indexOfRemoved;
            indexOfRemoved=rightOfRemoved;
        }else{
            Structure[indexOfRemoved] = Structure[leftOfRemoved];
            Structure[indexOfRemoved]->MinHeapIndex=indexOfRemoved;
            indexOfRemoved=leftOfRemoved;
        }
        rightOfRemoved = HeapRight(indexOfRemoved);
        leftOfRemoved = HeapLeft(indexOfRemoved);
    }

    
    Structure[indexOfRemoved]=Structure[DataStructureSize-1];
    DataStructureSize--;
    Structure[indexOfRemoved]->MinHeapIndex = indexOfRemoved;
    
    OrderElement(indexOfRemoved);
    return nodeToRemove;
}

int DijkstraTravelCost(CellPointer startCell,CellPointer destinationCell){
    for(CellPointer i=MatrixOfHexagonal;i<=endOfMatrix;i++){
        i->distance=endlessDistance;
    }
    
    startCell->distance=0;

    DataStructureSize=0;
    HeapInsert(startCell);

    CellPointer currentCell=NULL;

    int newDistanceCandidate=-1;

    CellPointer AdjacentCell = NULL;
    CellPointer AdjacentNextColumnCell = NULL;
    CellPointer AdjacentPrecColumnCell = NULL;

    int AdjacentSupportRow = 0;

    int currentAirRoute=0;
    
    int currentCellCoordinates=-1;

    int rowEvenOdd = -1;

    int AirRouteCount=-1;
    while(DataStructureSize>0){

        currentCell = HeapRemove();
        
        if(currentCell==destinationCell){
            return currentCell->distance; 
        }

        currentCellCoordinates = currentCell - MatrixOfHexagonal;

        AdjacentSupportRow = currentCellCoordinates % matrixRow;

        rowEvenOdd = AdjacentSupportRow % 2;

        AdjacentNextColumnCell = currentCell + matrixRow;
        AdjacentPrecColumnCell = currentCell - matrixRow;

        if(currentCell->cost>0){
            newDistanceCandidate = currentCell->distance + currentCell->cost;
            
            if(AdjacentPrecColumnCell > MatrixOfHexagonal){
                //LeftCell
                if (newDistanceCandidate < AdjacentPrecColumnCell->distance){
                    if(AdjacentPrecColumnCell->distance == endlessDistance){
                        AdjacentPrecColumnCell->distance=newDistanceCandidate;
                        HeapInsert(AdjacentPrecColumnCell);
                    }else{
                        AdjacentPrecColumnCell->distance=newDistanceCandidate;
                        OrderElement(AdjacentPrecColumnCell->MinHeapIndex);
                    }
                }

                if(rowEvenOdd ==0){
                    if((AdjacentSupportRow+1) < matrixRow){
                        AdjacentCell = AdjacentPrecColumnCell+1;
                        if (newDistanceCandidate < AdjacentCell->distance){ 
                            if(AdjacentCell->distance == endlessDistance){   
                                AdjacentCell->distance=newDistanceCandidate;
                                HeapInsert(AdjacentCell);
                            }else{
                                AdjacentCell->distance=newDistanceCandidate;
                                OrderElement(AdjacentCell->MinHeapIndex);
                            }
                        }
                    
                    }
                    
                    if(AdjacentSupportRow >0){
                        AdjacentCell = AdjacentPrecColumnCell-1;
                        if (newDistanceCandidate < AdjacentCell->distance){ 
                            if(AdjacentCell->distance == endlessDistance){
                                AdjacentCell->distance=newDistanceCandidate;
                                HeapInsert(AdjacentCell);
                            }else{
                                AdjacentCell->distance=newDistanceCandidate;
                                OrderElement(AdjacentCell->MinHeapIndex);
                            }
                        }
                    }
                }
            }

            //Casi in cui la colonna è la stessa
            if((AdjacentSupportRow+1) < matrixRow){
                AdjacentCell=currentCell+1;
                if (newDistanceCandidate < AdjacentCell->distance){
                    if(AdjacentCell->distance == endlessDistance){
                        AdjacentCell->distance=newDistanceCandidate;
                        HeapInsert(AdjacentCell);
                    }else{
                        AdjacentCell->distance=newDistanceCandidate;
                        OrderElement(AdjacentCell->MinHeapIndex);
                    }
                }
            }
            
            if(AdjacentSupportRow > 0){
                AdjacentCell=currentCell-1;
                if (newDistanceCandidate < AdjacentCell->distance){
                    if(AdjacentCell->distance == endlessDistance){
                        AdjacentCell->distance=newDistanceCandidate;
                        HeapInsert(AdjacentCell);
                    }else{
                        AdjacentCell->distance=newDistanceCandidate;
                        OrderElement(AdjacentCell->MinHeapIndex);
                    }
                }
            }
            
            if(AdjacentNextColumnCell < endOfMatrix){
                //RightCell
                if (newDistanceCandidate < AdjacentNextColumnCell->distance){
                    if(AdjacentNextColumnCell->distance == endlessDistance){
                        AdjacentNextColumnCell->distance=newDistanceCandidate;
                        HeapInsert(AdjacentNextColumnCell);
                    }else{
                        AdjacentNextColumnCell->distance=newDistanceCandidate;
                        OrderElement(AdjacentNextColumnCell->MinHeapIndex);
                    }
                }

                if(rowEvenOdd == 1){
                    if ((AdjacentSupportRow+1) < matrixRow){
                        AdjacentCell = AdjacentNextColumnCell + 1;
                        if (newDistanceCandidate < AdjacentCell->distance){
                            if(AdjacentCell->distance == endlessDistance){
                                AdjacentCell->distance=newDistanceCandidate;
                                HeapInsert(AdjacentCell);
                            }else{
                                AdjacentCell->distance=newDistanceCandidate;
                                OrderElement(AdjacentCell->MinHeapIndex);
                            }
                        }
                    }

                    if(AdjacentSupportRow > 0){
                        AdjacentCell = AdjacentNextColumnCell - 1;
                        if (newDistanceCandidate < AdjacentCell->distance){
                            if(AdjacentCell->distance == endlessDistance){
                                AdjacentCell->distance=newDistanceCandidate;
                                HeapInsert(AdjacentCell);
                            }else{
                                AdjacentCell->distance=newDistanceCandidate;
                                OrderElement(AdjacentCell->MinHeapIndex);
                            }
                        }
                    }
                }
            }

            AirRouteCount = currentCell->AirRouteNumber;
            currentAirRoute=0;
            while(currentAirRoute<AirRouteCount){
                AdjacentCell = MatrixOfHexagonal + currentCell->AirRoute[currentAirRoute];

                if (newDistanceCandidate < AdjacentCell->distance){
                    if(AdjacentCell->distance == endlessDistance){
                        AdjacentCell->distance=newDistanceCandidate;
                        HeapInsert(AdjacentCell);
                    }else{
                        AdjacentCell->distance=newDistanceCandidate;
                        OrderElement(AdjacentCell->MinHeapIndex);
                    }
                }
                currentAirRoute++;
            }
        }
    }
    return -1;
};

int TravelCost(int startCellColumn,int startCellRow,int destinationCellColumn,int destinationCellRow){
    if(startCellColumn==destinationCellColumn && startCellRow==destinationCellRow){
        return 0;
    }
    CellPointer startCell = MatrixOfHexagonal + startCellColumn*matrixRow+startCellRow;
    CellPointer destinationCell = MatrixOfHexagonal + destinationCellColumn*matrixRow+destinationCellRow;
    int cost = DijkstraTravelCost(startCell,destinationCell);
    return cost;
};

void Enqueue(CellPointer coordinate){
    
    if(queueHead==queueTail+1 || (queueHead==0 && queueTail==ArraySize-1)){
        queueHead=0;
        queueTail=ArraySize-1;
        ArraySize = ArraySize*2;
        Structure = (DataStructurePointer)realloc(Structure,sizeof(CellPointer)*ArraySize);
    }
    
    Structure[queueTail]=coordinate;
    if(queueTail==ArraySize-1){
        queueTail=0;
    }else{
        queueTail=queueTail+1;
    }

}
CellPointer Dequeue(){
    CellPointer coordinate=Structure[queueHead];
    if(queueHead==ArraySize-1){
        queueHead=0;
    }else{
        queueHead=queueHead+1;
    }
    return coordinate;
}

void ChangeCost(int startCellColumn,int startCellRow,float v, float range){
    
    for(CellPointer i=MatrixOfHexagonal;i<endOfMatrix;i++){
        i->distance=notChanged;
    }
    
    int coordinates = startCellColumn * matrixRow + startCellRow;

    CellPointer currentCell = MatrixOfHexagonal + coordinates;
    float newCost = 0;
    float currentDistance = 0;
    float fantasyNumber = 0;

    queueTail=0;
    queueHead=0;

    currentCell->distance=0;

    Enqueue(currentCell);

    int AdjacentSupportRow = -1;

    int rowEvenOdd = -1;

    CellPointer AdjacentCell = NULL;
    CellPointer AdjacentNextColumnCell = NULL;
    CellPointer AdjacentPrecColumnCell = NULL;

    while(queueHead!=queueTail){
        currentCell = Dequeue();
        
        currentDistance = currentCell->distance;

        fantasyNumber = ((range - currentDistance)/range);

        newCost = ((float)currentCell->cost) + v * fantasyNumber;

        if(newCost<minCost){
            newCost=minCost;
        }else if(newCost>maxCost){
            newCost=maxCost;
        }
        currentCell->cost=(int)newCost;
        
        currentDistance++;
        
        if(currentDistance<range){

            coordinates = currentCell-MatrixOfHexagonal;

            AdjacentSupportRow = coordinates % matrixRow;
            
            rowEvenOdd = AdjacentSupportRow % 2;

            AdjacentNextColumnCell = currentCell + matrixRow;
            AdjacentPrecColumnCell = currentCell - matrixRow;

            if(AdjacentPrecColumnCell > MatrixOfHexagonal){
                
                AdjacentCell = AdjacentPrecColumnCell;
                
                if(AdjacentCell->distance==notChanged){
                    AdjacentCell->distance=currentDistance;
                    Enqueue(AdjacentCell);
                }

                if(rowEvenOdd==0){
                    if(AdjacentSupportRow > 0){
                        AdjacentCell = AdjacentPrecColumnCell-1;
                        if(AdjacentCell->distance==notChanged){
                            AdjacentCell->distance=currentDistance;
                            Enqueue(AdjacentCell);
                        }
                    }

                    if((AdjacentSupportRow+1) < matrixRow){
                        AdjacentCell = AdjacentPrecColumnCell+1;
                        if(AdjacentCell->distance==notChanged){
                            AdjacentCell->distance=currentDistance;
                            Enqueue(AdjacentCell);
                        }
                    }
                }
            }

            if(AdjacentSupportRow > 0){
                AdjacentCell = currentCell-1;
                if(AdjacentCell->distance==notChanged){
                    AdjacentCell->distance=currentDistance;
                    Enqueue(AdjacentCell);
                }
            }

            if((AdjacentSupportRow+1) < matrixRow){
                AdjacentCell = currentCell+1;
                if(AdjacentCell->distance==notChanged){
                    AdjacentCell->distance=currentDistance;
                    Enqueue(AdjacentCell);
                }
            }

            if(AdjacentNextColumnCell < endOfMatrix){
                if(AdjacentNextColumnCell->distance==notChanged){
                    AdjacentNextColumnCell->distance=currentDistance;
                    Enqueue(AdjacentNextColumnCell);
                }
                if(rowEvenOdd == 1){
                    if(AdjacentSupportRow > 0){
                        AdjacentCell = AdjacentNextColumnCell - 1;
                        if(AdjacentCell->distance==notChanged){
                            AdjacentCell->distance=currentDistance;
                            Enqueue(AdjacentCell);
                        }
                    }

                    if((AdjacentSupportRow+1) < matrixRow){
                        AdjacentCell = AdjacentNextColumnCell + 1;
                        if(AdjacentCell->distance==notChanged){
                            AdjacentCell->distance=currentDistance;
                            Enqueue(AdjacentCell);
                        }
                    }
                }
            }
        }
    }
}
int readParameter(){
    int parameter=0;
    if(scanf("%d",&parameter)==1){
        return parameter;
    }
    return -1;
};


int main(){
    int startCellColumn=-1;
    int startCellRow=-1;
    int destinationCellColumn=-1;
    int destinationCellRow=-1;
    int v=-11;
    int range=-1;
    
    int row=-1;
    int column=-1;

    char comand[20];

    char endRow='\n';

    int returnValor=-1;
    while(feof(stdin)==0){
        if(scanf("%s",comand)==1){
            if(strcmp(comand,"travel_cost")==0){
                startCellColumn=readParameter();
                startCellRow=readParameter();
                destinationCellColumn=readParameter();
                destinationCellRow=readParameter();
                if(startCellRow<0 || startCellRow>=matrixRow || startCellColumn<0 || startCellColumn>=matrixColumn
                    || destinationCellRow<0 || destinationCellRow>=matrixRow || destinationCellColumn<0 || destinationCellColumn>=matrixColumn){
                    printf("-1");
                }else{
                    returnValor=TravelCost(startCellColumn,startCellRow,destinationCellColumn,destinationCellRow);
                    printf("%d",returnValor);
                }
                startCellColumn=-1;
                startCellRow=-1;
                destinationCellColumn=-1;
                destinationCellRow=-1;
            }else if(strcmp(comand,"init")==0){
                column=readParameter();
                row=readParameter();
                if(row<=0 || column<=0){
                    printf("KO");
                }else{    
                    init(row,column);
                }
                column=-1;
                row=-1;
            }else if(strcmp(comand,"change_cost")==0){
                startCellColumn=readParameter();
                startCellRow=readParameter();
                v=readParameter();
                range=readParameter();

                if(startCellRow<0 || startCellRow>=matrixRow || startCellColumn<0 || startCellColumn>=matrixColumn
                    || range <= 0 
                    || v > maxV || v < minV){
                    printf("KO");
                }else if(v==0){
                    printf("OK");
                }else{
                    ChangeCost(startCellColumn,startCellRow,(float)v,(float)range);
                    printf("OK");
                }
                startCellColumn=-1;
                startCellRow=-1;
                v=-11;
                range=-1;
            }else if(strcmp(comand,"toggle_air_route")==0){
                startCellColumn=readParameter();
                startCellRow=readParameter();
                destinationCellColumn=readParameter();
                destinationCellRow=readParameter();

                if(startCellRow<0 || startCellRow>=matrixRow || startCellColumn<0 || startCellColumn>=matrixColumn
                    || destinationCellRow<0 || destinationCellRow>=matrixRow || destinationCellColumn<0 || destinationCellColumn>=matrixColumn){
                     printf("KO");
                }else{
                    ToggleAirRoute(startCellRow,startCellColumn,destinationCellRow,destinationCellColumn);
                }
            }

            if(scanf("%c",&endRow)==1){
                printf("%c",endRow);
            }
        }
    }
    free(CellNULL);
    free(Structure);
    freeMatrixOfHexagonal();
    return 0;
}