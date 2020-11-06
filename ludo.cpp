#include <iostream>
#include <vector>
#include <string>
#include  <utility>
using namespace std;

#define BLUE 0
#define RED 1
#define GREEN 2
#define YELLOW 3

#define START 0
#define PATH 1
#define HOME 2

#define START_POS 99
#define SAFE 1
#define NOT_SAFE 0

int limiters[4][2] = {{0,50},{13,11},{26,24},{39,37}};
string headers[4] = {"BLUE_","RED_","GREEN_","YELLOW_"};

/*
path_pos < 0 signifies that coin is in home area of the corresponding color
path_pos =99 is START area
0<=path_pos<=51 signifies PATH area
BLUE -> from 0 to 50
RED -> from 13 to 11
GREEN -> from 26 to 24
YELLOW -> from 39 to 37
safe zone locations = 0,13,26,39,        8,21,34,47
*/

class Coin
{
    private:
        int path_pos;
        string UID;
        bool isSafe;
        int area;
        int color;
    public:
        Coin(string UID,int color)
        {
            this->color = color;
            path_pos = START_POS;
            isSafe = 1;
            area = START;
            this->UID = UID;
        }

        void gotKilled()
        {
            path_pos = START_POS;
            isSafe = 1;
            area = START;       
        }

        void updatePathPos(int path_pos,bool isSafe,int area)
        {
           this->path_pos = path_pos;
           this->isSafe = isSafe;
           this->area = area;
        }        

        void goToWar(int path_pos)
        {
            this->path_pos = path_pos;
            isSafe = 1;
            area = PATH;            
        }

        int getArea()
        {
            return area;
        }

        int getPathPos()
        {
            return path_pos;
        }

        string getUID()
        {
            return UID;
        }
};

class Color
{
    vector<Coin> Coins;
    int color;
    int n_done;
    public:
    Color(int color)
    {
        this->color = color;
        n_done = 0;

        string s = headers[color];
        for(int i=0;i<4;i++)
        {
            s+= (int)49+i;
            Coins.push_back(Coin(s,color));
            s.erase(s.end()-1);
        }
    }

    void killCoinsAtPos(int pos)
    {
        for(int i=0;i<4;i++)
        {
            if(pos==Coins[i].getPathPos())
                Coins[i].gotKilled();
        }
    }

    vector<pair<string,int>> getAllPos()
    {
        vector<pair<string,int>> res;
        for(int i=0;i<4;i++)
        {
            res.push_back(make_pair(Coins[i].getUID(),Coins[i].getPathPos()));
        }

        return res;
    }

    int getPathPos(string coin_id)
    {
        return Coins[(int)coin_id[coin_id.length()-1]-49].getPathPos();
    }

    void goToWar(string coin_id)
    {
        return Coins[(int)coin_id[coin_id.length()-1]-49].goToWar(limiters[color][0]);
    }

    void updatePos(string coin_id,int path_pos,bool isSafe,int area)
    {
        return Coins[(int)coin_id[coin_id.length()-1]-49].updatePathPos(path_pos,isSafe,area);
    }

    void updatePosReachedHome(string coin_id)
    {
        n_done++;
        Coins[(int)coin_id[coin_id.length()-1]-49].updatePathPos(-6,1,HOME);
    }

    int coinsReachedHome()
    {
        return n_done;
    }  

    bool areCoinsPresent(int pos)
    {
        for(int i=0;i<4;i++)
        {
            if(Coins[i].getPathPos()==pos)
                return 1;
        }
        return 0;
    }     

    bool areOtherCoinsPresent(string coin_id, int pos)
    {
        for(int i=0;i<4;i++)
        {
            if(i==(int)coin_id[coin_id.length()-1]-49)
                continue;
            if(Coins[i].getPathPos()==pos)
                return 1;
        }
        return 0;
    }     
};

bool isSafeZone(int coin_pos){
    return ((coin_pos%13==0) || ((coin_pos+5)%13==0));
}

//Returns re serve potential
bool updater(int index, vector<Color> &Colors,string coin_id,int dice,vector<int> &colorChecker)
{
    Color &color = Colors[index];
    int coin_pos = color.getPathPos(coin_id);
    if(coin_pos==99)
    {
        if(dice==6)
        {
            color.goToWar(coin_id);
            return 1;
        }
        else
        {
            cout<<"Coin not movable without 6........TURN SKIPPED......"<<endl;
            return 0;
        }        
    }
    else if(coin_pos<0)
    {
        if(coin_pos-dice<-6)
        {
            cout<<"Coin not movable ........TURN SKIPPED......"<<endl;
            return 0;
        }
        else if(coin_pos-dice==-6)
        {
            cout<<"Coin named "<<coin_id<<" has reached its home"<<endl;
            color.updatePosReachedHome(coin_id);
            if(color.coinsReachedHome()==4)
            {
                cout<<"Game Over........Player "<<index+1<<" is the winner"<<endl;
                cout<<"Exiting"<<endl;
                exit(0);
            }
            return 1;
        }
        else
        {
            color.updatePos(coin_id,coin_pos-dice,SAFE,HOME);
            return 0;
        }
        
    }   

    //Coin is always movable from here
    if(!isSafeZone(coin_pos) && !color.areOtherCoinsPresent(coin_id,coin_pos))
            colorChecker[coin_pos] = -1;

    if(coin_pos == limiters[index][1]&&dice==6)
    {
        cout<<"Coin named "<<coin_id<<" has reached its home"<<endl;
        color.updatePosReachedHome(coin_id);
        if(color.coinsReachedHome()==4)
        {
            cout<<"Game Over........Player "<<index<<" is the winner"<<endl;
            cout<<"Exiting"<<endl;
            exit(0);
        }        
        return 1;
    }
    if(coin_pos <= limiters[index][1] && coin_pos+dice>limiters[index][1])
    {
        color.updatePos(coin_id,-(coin_pos+dice-limiters[index][1]),SAFE,HOME);
       if(dice==6)
            return 1;
        return 0;
    }

    if(isSafeZone((coin_pos+dice)%52))
    {
        color.updatePos(coin_id,(coin_pos+dice)%52,SAFE,PATH);
         if(dice==6)
            return 1;
        return 0;
    }
    else
    {
        color.updatePos(coin_id,(coin_pos+dice)%52,NOT_SAFE,PATH);
        if(colorChecker[(coin_pos+dice)%52]==index)
        {
            if(dice==6)
                return 1;
            return 0;
        }
        else if(colorChecker[(coin_pos+dice)%52]==-1)
        {
            colorChecker[(coin_pos+dice)%52] = index;
            if(dice==6)
                return 1;
            return 0;
        }
        else
        {
            Colors[colorChecker[(coin_pos+dice)%52]].killCoinsAtPos((coin_pos+dice)%52);
            colorChecker[(coin_pos+dice)%52] = index;            
            return 1;
        }
    }
    if(dice==6)
        return 1;
    return 0;
}

int main()
{

    int n_p = 0;
    cout<<"Enter number of players :: "<<endl;
    cin>>n_p;
    vector<Color> Colors;
    for(int i=0;i<n_p;i++)
        Colors.push_back(Color(i));

    vector<vector<pair<string,int>>> getAllPosforColors;

    int index = 0;
    string coin_id;
    int dice = 0;

    vector<int> colorChecker(52,-1);

    while(1)
    {
        getAllPosforColors.clear();
        for(int j = 0;j<n_p;j++)
            getAllPosforColors.push_back(Colors[j].getAllPos());

        for(int j = 0;j<n_p;j++)        
            for(int i = 0;i<4;i++)
                cout<<getAllPosforColors[j][i].first<<"is At "<<getAllPosforColors[j][i].second<<endl;

        for(int i=0;i<52;i++)
            cout<<colorChecker[i]<<" ";
        cout<<endl;

        cout<<"Player "<<index+1<<" press Enter to roll a dice : ";
        cin.get();
        cin.get();
        dice = rand()%6 + 1;
        //dice = 6;
        cout<<"Value on dice = "<<dice<<endl;

        // cout<<"Player "<<index+1<<" Enter dice value : ";
        // cin>>dice;

        cout<<"Please Enter Coin ID you would like to move : ";
        cin>>coin_id;       


        if(updater(index,Colors,coin_id,dice,colorChecker))
            continue;
            
        index = (index+1)%n_p;
    }
}