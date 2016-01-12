int main() {
    int *a = NULL;
    // 150M
    int v = 150000000;
    a = (int *) malloc(v);
    if (a == NULL) {
        printf("error\n");
    }
    else {
        memset(a, 0, v);
        printf("success\n");
    }
    return 15;
}
