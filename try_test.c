#include <stdio.h>

int main()
{
    int n;  //Number of rows
    scanf("%d", &n);

    char str[n][100];
    for(int i = 0; i < n; ++i)
    {
        scanf("%s", str[i]);
    }

    for(int i = 0; i < n; ++i)
    {
        printf("%s\n", str[i]);
    }
}
