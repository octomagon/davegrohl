#include <iostream>
using namespace std;


int ask(int i)
{
    cout << 1 << " " << i << endl;
    int v;
    cin >> v;
    return v;
}
void ans(int i)
{
    cout << 2 << " " << i << endl;
}

int main()
{
    int t;
    cin >> t;

    while(t-- > 0)
    {
        int n, x;
        cin >> n >> x;


        // find app range containing x
        int p = 0;
        int r = 20;
        int i = 0;
        if(n != -1)
        {
            r = 1;
            while((1<<r) <= n){r++;}
        }


        // p is always less
        int v = ask(1);
        if(v == x){i = 1;}
        else if(v == 0){i = -1;}
        // r is always high



        int ind = 1;
        while(i == 0 && r-p > 1)
        {
            int q = (p+r)/2;
            v = ask(ind|(1<<q));
            if(v == x){i = ind|(1<<q);break;}
            else if(v != 0 && v < x){ind |= (1<<q);p = q;}
            else{r = q;}
        }


        p = ind;
        r = ind|(1<<r);
        while(i == 0 && r-p+1 > 3)
        {
            int q = (p+r)/2;
            if(q%2 == 0){q--;}

            v = ask(q);
            if(v == x){i=q;break;}
            else if(v == 0){r = q;}
            else if(v < x){p = q;}
            else if(v > x){r = q;}
        }


        if(i != 0){ans(i);}
        else
        {
            p++;
            int v = ask(p);

            int c = 1;
            while((p&c) == 0)
            {
                int t = 0;
                t = ask(p|c);
                v -= t;
                c <<= 1;
            }

            if(v == x){i=p;}
            else{i = -1;}

            ans(i);
        }
        
        string c;
        cin >> c;
        if(c == "No"){break;}
    }
}

