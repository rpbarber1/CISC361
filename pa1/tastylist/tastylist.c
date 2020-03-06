#include<stdio.h>
#include<stdlib.h>
 
#include "tastylist.h"
 
int  main()
{
    int i,num;
    struct node *n;
    head=NULL;
    while(1)
    {
    printf("\nList Operations\n");
    printf("===============\n");
    printf("1.Insert\n");
    printf("2.Display\n");
    printf("3.Size\n");
    printf("4.Delete\n");
    printf("5.Exit\n");
    printf("Enter your choice : ");
    if(scanf("%d",&i)<=0){
        printf("Enter only an Integer\n");
        exit(0);
    } else {
        switch(i)
        {
        case 1:      printf("Enter the number to insert : ");
                 scanf("%d",&num);
                 insert(num);
                 break;
        case 2:     if(head==NULL)
                {
                printf("List is Empty\n");
                }
                else
                {
                printf("Element(s) in the list are : ");
                }
                display(n);
                break;
        case 3:     printf("Size of the list is %d\n",count());
                break;
        case 4:     if(head==NULL)
                printf("List is Empty\n");
                else{
                printf("Enter the number to delete : ");
                scanf("%d",&num);
                if(delete(num))
                    printf("%d deleted successfully\n",num);
                else
                    printf("%d not found in the list\n",num);
                }
                break;
        case 5:     return 0;
        default:    printf("Invalid option\n");
        }
    }
    }
    return 0;
}
