int stupid_recursive(int i, int j, int k)
{
    return stupid_recursive(stupid_recursive(k, i, j), j + k, i);
}

int main()
{
    return stupid_recursive(0, 0, 0);
}
