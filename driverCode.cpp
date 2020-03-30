#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <queue>
#include <list>
#include <sstream>
#include <algorithm>
#include <stack>
#include <bits/stdc++.h>
#include <map>
#include <set>
#include <cmath>
#include <string>

using namespace std;

typedef unsigned long long ull;
typedef long long ll;
typedef pair<int, int> ii;
typedef vector<int> vi;
typedef vector<int, int> vii;
typedef map<string, int> msi;
typedef set<int> si;

#define trvi(c, it) for (vi::iterator it = (c).begin(); it != (c).end(); ++it)
#define trvii(c, it) for (vii::iterator it = (c).begin(); it != (c).end(); ++it)
#define trmsi(c, it) for (msi::iterator it = (c).begin(); it != (c).end(); ++it)
#define trsi(c, it) for (si::iterator it = (c).begin(); it != (c).end(); ++it)


#define debug(x) cout << '>' << #x << ':' << x << endl;
#define rep(i,n) for(int i=0;i<(n);++i)
#define fori(i,a,b) for(int i=(a);i<=(b);++i)
#define ford(i,a,b) for(int i=(a);i>=(b);--i)
#define mod 1000000007

// To find GCD of a and b
ll gcd(ll a, ll b);

// To compute x raised to power y under modulo m
ll power(ll x, ll y, ll m);

ll inv(ll a, ll b);

ll fact(ll num)
{
    ll prod = 1;
    fori(i, 1, num)
    {
        prod = prod*i%mod;
    }
    return prod;
}

//size of array is l
ll factorial_arr(ll fac[], ll l)
{
    fac[0] = 1;
    fori(i, 1, l-1)
    {
       fac[i] = fac[i-1]*i; 
    }
}

//Find inverse factorial i.e. inv(2!), inv(3!), inv(4!) and so on... and store in array
void find_inv_fact(ll inv_fac[], ll l)
{
    inv_fac[0] = 1ULL;
    inv_fac[1] = 1ULL;
    fori(i, 2, l)
    {
        inv_fac[i] = (inv_fac[i-1]*inv(i, mod))%mod;
    }
}

//Find a invere mod m
ll inv(ll a, ll m)
{
     return power(a, m-2, m);
}

// To compute x^y under modulo m
ll power(ll x,  ll y,  ll m)
{
    if (y == 0)
        return 1;
    ll p = power(x, y/2, m) % m;
    p = (p * p) % m;

    return (y%2 == 0)? p : (x * p) % m;
}

// Function to return gcd of a and b
ll gcd(ll a, ll b)
{
    if (a == 0)
        return b;
    return gcd(b%a, a);
} 


int main()
{

 return 0;
}

