long int fac(long int n)
{
    long int result = 1;

    for (long int i = 2; i <= n; i++) {
        result *= i;
    }

    return result;
}

