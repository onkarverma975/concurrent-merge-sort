#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include<time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void selectionSort(int arr[], int n);
void mergeSortNon(int arr[], int l, int r);
void mergeSort(int a[], int l, int h);
void mergeNon(int arr[], int l, int m, int r);
void merge(int a[], int l1, int h1, int h2);
// Driver code
int main()
{
	int shmid;
	key_t key = IPC_PRIVATE;
	int *shm_array;


	int n;

	scanf("%d",&n);

	size_t SHM_SIZE = sizeof(int)*n;
	//create a shared memory page
	if ((shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666)) < 0)
	{
		fprintf(stderr,"Error in 'shmget'\n");
		exit(1);
	}
	//create the page operable by the processes
	if ((shm_array = shmat(shmid, NULL, 0)) == (int *) -1)
	{
		fprintf(stderr,"Error in 'shmat'\n");
		exit(1);
	}
	int it;
	for(it=0;it<n;it++)
		scanf("%d",&shm_array[it]);
	int copy[n];
	for(it=0;it<n;it++)
		copy[it]=shm_array[it];
	clock_t begin = clock();

	/* here, do your time-consuming job */

	mergeSort(shm_array, 0, n-1);

	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	for(it=0;it<n;it++)
		printf("%d ",shm_array[it]);
	printf("\n");
	printf("Time taken for concurrent execution is %lf\n",time_spent);

	begin = clock();
	mergeSortNon(copy, 0, n - 1);
	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("Time taken for Non-concurrent execution is %lf\n",time_spent);

	/* Detach from the shared memory now that we are
	   done using it. */
	if (shmdt(shm_array) == -1)
	{
		fprintf(stderr,"Error in 'shmdt'\n");
		exit(1);
	}

	/* Delete the shared memory segment. */
	if (shmctl(shmid, IPC_RMID, NULL) == -1)
	{
		fprintf(stderr,"Error in 'shmid'\n");
		exit(1);
	}


	return 0;
}
void mergeNon(int arr[], int l, int m, int r)
{
	int i, j, k;
	int n1 = m - l + 1;
	int n2 =  r - m;

	/* create temp arrays */
	int L[n1], R[n2];

	/* Copy data to temp arrays L[] and R[] */
	for (i = 0; i < n1; i++)
		L[i] = arr[l + i];
	for (j = 0; j < n2; j++)
		R[j] = arr[m + 1+ j];

	/* Merge the temp arrays back into arr[l..r]*/
	i = 0; // Initial index of first subarray
	j = 0; // Initial index of second subarray
	k = l; // Initial index of merged subarray
	while (i < n1 && j < n2)
	{
		if (L[i] <= R[j])
		{
			arr[k] = L[i];
			i++;
		}
		else
		{
			arr[k] = R[j];
			j++;
		}
		k++;
	}

	/* Copy the remaining elements of L[], if there
	   are any */
	while (i < n1)
	{
		arr[k] = L[i];
		i++;
		k++;
	}

	/* Copy the remaining elements of R[], if there
	   are any */
	while (j < n2)
	{
		arr[k] = R[j];
		j++;
		k++;
	}
}

void merge(int a[], int l1, int h1, int h2)
{
	int count=h2-l1+1;
	int sorted[count];
	int i=l1, k=h1+1, m=0;
	while (i<=h1 && k<=h2)
	{
		if (a[i]<a[k])
			sorted[m++]=a[i++];
		else if (a[k]<a[i])
			sorted[m++]=a[k++];
		else if (a[i]==a[k])
		{
			sorted[m++]=a[i++];
			sorted[m++]=a[k++];
		}
	}

	while (i<=h1)
		sorted[m++]=a[i++];

	while (k<=h2)
		sorted[m++]=a[k++];

	int arr_count = l1;
	for (i=0; i<count; i++,l1++)
		a[l1] = sorted[i];
}
void mergeSortNon(int arr[], int l, int r)
{
	if (l < r)
	{
		int m = l+(r-l)/2;
		mergeSortNon(arr, l, m);
		mergeSortNon(arr, m+1, r);

		mergeNon(arr, l, m, r);
	}
}

void mergeSort(int a[], int l, int h)
{
	int i, len=(h-l+1);

	// Using insertion sort for small sized array
	if (len<5)
	{
		selectionSort(a+l, len);
		return;
	}

	pid_t lpid,rpid;
	lpid = fork();
	if(lpid>0)
	{
		rpid = fork();
		if (rpid<0)
		{
			fprintf(stderr,"Right child not created\n");
			_exit(-1);
		}
		else if(rpid==0)
		{
			mergeSort(a,l+len/2,h);
			_exit(0);
		}
	}
	else if (lpid==0)
	{
		mergeSort(a,l,l+len/2-1);
		_exit(0);
	}
	else if (lpid<0)
	{
		fprintf(stderr,"Left child not created\n");
		_exit(-1);
	}
	int status;

	waitpid(rpid, &status, 0);
	waitpid(lpid, &status, 0);
	merge(a, l, l+len/2-1, h);
}

void selectionSort(int ar[], int n)
{
	int i,j,temp;
	for(i=0;i<n;i++){
		for(j=i+1;j<n;j++){
			if(ar[i]>ar[j]){
				temp=ar[i];
				ar[i]=ar[j];
				ar[j]=temp;
			}
		}
	}
}

