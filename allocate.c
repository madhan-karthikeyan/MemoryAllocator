#include <pthread.h>
#include <unistd.h>

typedef char ALIGN[16];

union header {
    struct memBlock // info of the memory block
    {
        size_t size;
        unsigned is_free;
        union header *next;
    } s;
    ALIGN stub; // Data being stored in
};
typedef union header header_t;

pthread_mutex_t global_malloc_lock;

header_t *head, *tail;

header_t *get_free_block(size_t size) {
    header_t *temp = head;
    while(temp) {
        if (temp->s.is_free && temp->s.size>=size) {
            return temp;
        }
        temp = temp->s.next;
    }
}

void *malloc(size_t size) {
    size_t total_size; // Total memory available to allocate
    void *block;
    header_t *header;
    if (!size) 
        return NULL;
    pthread_mutex_lock(&global_malloc_lock);
    header = get_free_block(size);
    if (header) {
        header->s.is_free = 0;
        pthread_mutex_unlock(&global_malloc_lock);
        return (void*)(header+1); // Encapsulating the memory adress
    }
    total_size = sizeof(header_t) + size;
    block = sbrk(total_size);
    if (block == (void*)-1) { // incase of memory unavailabe to allocate
        pthread_mutex_unlock(&global_malloc_lock);
        return NULL;
    }

    header = block;
    header->s.size = size;
    header->s.is_free = 0;
    header->s.next = NULL;
    if (!head)
        head = header;
    if (tail)
        tail->s.next = header;
    tail = header;
    pthread_mutex_unlock(&global_malloc_lock);
    return (void*)(header+1);
}

void free(void *block) {
    header_t *header, *temp;
    void *programBreak;

    if (!block)
        return;
    pthread_mutex_lock(&global_malloc_lock);
    header = (header_t*)block -1; // getting the real address
    programBreak = sbrk(0);
    if ((char*)block + header->s.size == programBreak) {
        if (head == tail) {
            head = tail = NULL;
        } else {
            temp = head;
            while (temp) {
                if (temp->s.next == tail) {
                    temp->s.next = NULL;
                    tail = temp;
                }
                temp = temp->s.next;
            }
        }
        sbrk(-sizeof(header_t) - header->s.size);
        pthread_mutex_unlock(&global_malloc_lock);
        return;
    }
    header->s.is_free = 1;
    pthread_mutex_unlock(&global_malloc_lock);
}

void *calloc(size_t num, size_t nsize) {
    size_t size;
    void *block;
    if (!num || !nsize)
        return NULL;
    size = num * nsize;
    block = malloc(size);
    if (!block)
        return NULL;
    memset(block, 0, size);
    return block;
}

void *realloc(void *block, size_t size) {
    header_t *header;
    void *ret;
    if (!block || !size) 
        return malloc(size);
    header = (header_t*)block -1;
    if (header->s.size >= size) 
        return block;
    ret = malloc(size);
    if (ret) {
        memcpy(ret, block, header->s.size);
        free(block);
    }
    return ret;
}

void print_mem_heap()
{
	// debug printing the memory heap
	header_t *curr = head;
	printf("head %p, tail = %p \n", (void *)head, (void *)tail);

	// fetching current node's header
	while (curr)
	{
		printf("addr: %p, size: %zu, is_free: %u, next: %p\n",
			   (void *)curr, curr->s.size, curr->s.is_free, (void *)curr->s.next);
		// traversing to the next node of the linked list
		curr = curr->s.next;
	}
}