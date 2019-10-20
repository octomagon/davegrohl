void insert(int item)
{
	if(full())
	{
		printf("Queue is overflowed.!!!\n");
		return;
	}
	if(front==-1)
	{
		front=0;
	}
	rear=rear+1;
	queuearr[rear]=item;
}
int del()
{
	int item;
	if(empty())
	{
		printf("Queue is underflowed...!!!\n");
		exit(1);
	}
	item=queuearr[front];
	front=front+1;
	return item;
}
int peek()
{
	if(empty())
	{
		printf("Queue is underflowed...!!!\n");
		exit(1);
	}
	return queuearr[front];
}
int empty()
{
	if(front==-1 || front==rear+1)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
int full()
{
	if(rear == max-1)
	{
		return 1;
	}
	else 
	{
		return 0;
	}
}
void display()
{
	int i;
	if(empty())
	{
		printf("Queue is empty\n");
		return;
	}
	printf("Queue element: ");
	for(i=front; i<=rear; i++)
	{
		printf("%d ",queuearr[i]);
	}
	printf("\n\n");
}
