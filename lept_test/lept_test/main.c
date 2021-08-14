//
//  main.c
//  lept_test
//
//  Created by Ember on 2021/8/7.
//  Copyright © 2021 Ember. All rights reserved.
//

#include <stdio.h>

int main(int argc, const char * argv[]) {
    // insert code here...
//    printf("Hello, World!\n""1\n");
//    double a  = 1.0;
//    printf("%5.2f",a);
    char *a = "test", *b = "est";
    int i;
    for(i=0;a[i];i++){
        if(b[i]!=a[i+1])
            printf("ERROR\n");
    }
    return 0;
}
