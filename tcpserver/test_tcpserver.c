#include <assert.h>
#include <stdio.h>

void test_smoke(void)
{
    assert(1 == 1);
}


int main(void)
{
    test_smoke();

    printf("=== All tests passed! ===\n");
}
