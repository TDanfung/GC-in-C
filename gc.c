#include <stdio.h>
#include <stdlib.h>

void riseException(const char* string) {
	printf(string);
}


typedef struct Vector {
	void** vector;
	int size;
	int index;
}Vector;

Vector* new_Vector(int initSize) {
	Vector* newVector = (Vector*)malloc(sizeof(Vector));
	if (newVector) {
		newVector->vector = (void**)malloc(sizeof(void*) * initSize);
		newVector->size = initSize;
		newVector->index = -1;
	}
}

void Vector_add(Vector* vector, void* elem) {
	if (!vector) {
		riseException("Vector is null");
		return;
	}

	if (vector->index == vector->size) {
		riseException("Vector is full");
		return;
	}

	vector->vector[++vector->index] = elem;	
}

void* Vector_get(Vector* vector, int index) {
	if (!vector) {
		riseException("Vector is null");
		return;
	}

	if (vector->index < index) {
		riseException("Selecected index is out of range");
		return;
	}

	return vector->vector[index];
}

int Vector_has(Vector* vector, void* elem) {
	int lastIndex = vector->index;
	for (int i = 0; i <= lastIndex; i++) {
		if (Vector_get(vector, i) == elem) {
			return 1;
		}
	}
	return 0;
}

typedef struct GCBlock {
	void* allocatedMemory;
	void** referedFrom;
	char isUsing;
}GCBlock;

typedef struct GC {
	Vector* gcblocks;
	Vector* pointersToFree;
	int size;
	int index;
}GC;

GC* new_GC(int initSize) {
	GC* newGC = (GC*)malloc(sizeof(GC));
	if (newGC) {
		newGC->gcblocks = new_Vector(initSize);
		newGC->pointersToFree = new_Vector(initSize);
	}
	return newGC;
}

void GC_addPointerToObserve(GC* gc, void* pointer, void** referedFrom) {
	if (!gc) {
		return;
	}
	GCBlock* newBlock = (GCBlock*)malloc(sizeof(GCBlock));
	if (newBlock) {
		newBlock->allocatedMemory = pointer;
		newBlock->referedFrom = referedFrom;
		newBlock->isUsing = 1;

		Vector_add(gc->gcblocks, newBlock);

	}
}

void GC_collect(GC* gc) {
	if (!gc) {
		return;
	}

	Vector* gcBlocks = gc->gcblocks;
	Vector* PTF = gc->pointersToFree; // Pointers To Free
	int lastIndex = gcBlocks->index;
	for (int i = 0; i <= lastIndex; i++) {

		GCBlock* currentGCblock = Vector_get(gcBlocks, i);
		if (currentGCblock->isUsing) {
			if (*currentGCblock->referedFrom == NULL) {
				Vector_add(PTF, currentGCblock->allocatedMemory);
				currentGCblock->isUsing = 0;
			}
			else if (Vector_has(PTF, currentGCblock->referedFrom) && !Vector_has(PTF,currentGCblock->allocatedMemory)) {

				//printf("GC에 담김 : %x \n", currentGCblock->allocatedMemory);

				Vector_add(PTF, currentGCblock->allocatedMemory);
				currentGCblock->isUsing = 0;
			}
		}
	}


	lastIndex = PTF->index;
	for (int i = 0; i <= lastIndex; i++) {
		void* toFree = Vector_get(PTF, i);
		free(toFree);
	}

}


//gc를 시험해보기 위한 객체

GC* gc = NULL;

typedef struct LNKL{
	int data;
	struct LNKL* next;
}LNKL;

LNKL* new_LNKL(LNKL** ptr) {
	LNKL* newLNKL = (LNKL*)malloc(sizeof(LNKL));
	if (newLNKL) {
		GC_addPointerToObserve(gc, newLNKL, ptr);
		newLNKL->data = -1;
		newLNKL->next = NULL;
	}
	return newLNKL;
}

void LNKL_addFirst(LNKL* linkedList, int data) {
	if (!linkedList) {
		riseException("Linked list is NULL");
		return;
	}
	LNKL* temp = linkedList->next;
	linkedList->next = new_LNKL(&linkedList->next);
	linkedList->next->next = temp;
}



int main() {
	
	gc = new_GC(64);
	LNKL* list = new_LNKL(&list);
	for (int i = 0; i < 32; i++) {
		LNKL_addFirst(list, i);
	}

	list = NULL;
	GC_collect(gc);

	return 0;
}





