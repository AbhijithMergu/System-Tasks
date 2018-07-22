#define PAGE_SIZE 1024
#define MAX_BLOCKS 12

typedef struct free_node{
	unsigned start_offset;
	unsigned end_offset;
	struct free_node* next;
}free_node;

typedef struct block{
	void* memory;
	unsigned free_size;
	free_node* head;
	struct block* next;

}block;
block* blocks;
int block_count = 0;

/*

Creates a free_list node.

Params :	start_add	-	start address for the free node.
end_add		-	end address for the free node.

Return value: free_list node created.

*/
free_node* create_free_list_node(unsigned start_add, unsigned end_add)
{

	free_node* node = (free_node*)malloc(sizeof(free_node));
	node->start_offset = start_add;
	node->end_offset = end_add;
	node->next = NULL;
	return node;

}


/*
Creates a block node when memory isn't sufficient for the manager.

Return value : A new block with 1KB memory assigned.
*/
block* create_block_node(){
	block_count += 1;
	block* block_node = (block*)malloc(sizeof(block));
	block_node->memory = malloc(PAGE_SIZE);
	block_node->free_size = PAGE_SIZE;
	block_node->head = create_free_list_node(0, PAGE_SIZE);
	block_node->next = NULL;
	return block_node;
}

/*

	Returns true if a block contains enough space to fit the req_size.

	false if a block doesn't contain enough space to fit the req_size.

*/
bool check_fit(block* block_node, unsigned req_size){
	
	if (block_node == NULL)
		return false;
	if (block_node->free_size < req_size)
	{
		return false;
	}
	free_node* node = block_node->head;

	while (node!=NULL&&(node->end_offset - node->start_offset < req_size))
	{
		node = node->next;
	}
	if (node != NULL)
		return true;
	return false;
}


/*
Gets a block node, traverses through the free list and assigns the required memory and

*/
void* assign_rq_mem(block* block_node, unsigned req_size){

	free_node* node = block_node->head;
	while (node->end_offset - node->start_offset < req_size)
	{
		node = node->next;
	}

	void* memory = (void*)((char*)block_node->memory + node->start_offset);
	*((short*)memory) = (short)req_size;
	block_node->free_size -= (req_size + 2);
	node->start_offset += (req_size + 2);
	return (void*)((char*)(memory)+2);

}

/*
A method to handle malloc request made by the user and return the memory requested for.

Return value : Memory requested for (OR) "NULL" if memory cannot be assigned.

*/

void* handle_malloc(unsigned req_size){

	if (req_size > PAGE_SIZE){
		return NULL;
	}
	if (blocks == NULL)
	{
		blocks = create_block_node();
		return assign_rq_mem(blocks, req_size);
	}
	
	int count = 0;
	block* temp = blocks;

	while (!check_fit(temp, req_size))
	{
		if (temp == NULL)
			break;
		temp = temp->next;
	}
	if (temp == NULL)
	{
		if (block_count >= MAX_BLOCKS)
		{
			printf("OUT OF MEMORY ERROR!\n");
			return NULL;
		}
		else
		{
			temp = blocks;
			if (temp == NULL)
			{
				temp = create_block_node();
			}
			else
			{
				while (temp->next != NULL)
				{
					temp = temp->next;
				}
				temp->next = create_block_node();
				temp = temp->next;
			}
			
		}
	}
	return assign_rq_mem(temp, req_size);
}

/*

Checks and returns which block the pointer address belongs to.

Param : the pointer called through free.

Return value : Block to which pointer belongs to.

*/
block* check_for_block(void* ptr)
{
	block* block_node = blocks;
	if (block_node == NULL)
		return NULL;
	/*while (!(block_node->memory <= ptr && (void*)((char*)block_node->memory + PAGE_SIZE) >= ptr))
		block_node = block_node->next;*/
	while (!((int)block_node->memory <= (int)ptr && (int)(void*)((char*)block_node->memory) + 1024) >= (int)ptr)
		block_node = block_node->next;
	return block_node;

}

/* 

	Merges the free list with the present free node.
	
*/
void merge_free_node(block* block_node, free_node* node){

	free_node* temp = block_node->head;
	free_node* previous = NULL;
	if (temp == NULL)
	{
		block_node->head = node;
		return;
	}
	while (node->start_offset < temp->start_offset)
	{
		previous = temp;
		temp = temp->next;
		if (temp == NULL)
			break;
	}
	if (previous != NULL)
	{
		if (previous->end_offset == node->start_offset)
		{
			previous->end_offset = node->end_offset;
			node = previous;
		}
		else
		{
			node->next = previous->next;
			previous->next = node;
			node->next = temp;
		}
	}
	else
	{
		node->next = temp;
		block_node->head = node;
	}
	
	if (temp!=NULL&&temp->start_offset == node->end_offset)
	{
		node->end_offset = temp->end_offset;
		node->next = temp->next;
		//free(temp);
	}
	block_node->free_size += (node->end_offset - node->start_offset);

}

/*
	
	Frees the empty block.

*/
void free_empty_block(block* block_node)
{
	block* temp = blocks;
	if (temp == block_node)
	{
		blocks = blocks->next;
		free(block_node);
		return;
	}
	while (temp->next != block_node)
		temp = temp->next;
	if (block_node->next == NULL)
	{
		temp->next = NULL;
		free(block_node);
	}
	else
	{
		temp->next = block_node->next;
		free(block_node);
	}
	block_count--;
	
}
/*
	Function to handle free requests.

*/
void handle_free(void* ptr){

	short size = *((short*)ptr - 1);
	block* block_node = check_for_block(ptr);
	
	int start_offset = (int)((char*)ptr-2-(char*)block_node->memory);
	int end_offset = start_offset + size + 2;
	free_node* node = create_free_list_node(start_offset, end_offset);

	merge_free_node(block_node, node);

	if (block_node->free_size == PAGE_SIZE)
		free_empty_block(block_node);
}

/* 
	Wrapper function for free and alloc.

*/
void* handle_req(char type, unsigned req_size, void* ptr){

	if (type == 'M')
	{
		return handle_malloc(req_size);
	}
	else {
		handle_free(ptr);
	}
	return NULL;

}

/*
	
	Function which to be used by the user to assigned req_size bytes of memory;

*/
void* mac_alloc(unsigned req_size){

	return handle_req('M', req_size, NULL);

}


/*
	
	Function which is to used by the user to free a pointer whose memory is previously assigned.

*/
void mac_free(void* ptr){

	handle_req('F', -1, ptr);
}