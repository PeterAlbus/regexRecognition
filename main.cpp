#include <iostream>
#include <map>
#include <utility>
#include <vector>
#include <set>
#include <stack>
#include <list>
#include <fstream>
#include <iomanip>

#define EPSILON '`'
using namespace std;

class NFANode
{
    int state;
    map<char,set<NFANode*>> nextMap;
public:
    NFANode(int state)
    {
        this->state=state;
    }
    void addNext(char c,NFANode* next)
    {
        this->nextMap[c].insert(next);
    }
    void setNext(map<char,set<NFANode*>> existNextMap)
    {
        this->nextMap=std::move(existNextMap);
    }
    bool isEnd()
    {
        if(nextMap.empty())
        {
            return true;
        }
        return false;
    }
    NFANode* getEnd()
    {
        set<int> outedStates;
        NFANode* ptr=this;
        while(!ptr->isEnd())
        {
            outedStates.insert(ptr->getState());
            for(const auto& itr:ptr->getNextMap())
            {
                for(auto it:itr.second)
                {
                    if(!outedStates.count(it->getState()))
                    {
                        ptr=it;
                        continue;
                    }
                }
            }
        }
        return ptr;
    }
    NFANode* next()
    {
        auto itr=nextMap.begin();
        return *(itr->second.begin());
    }
    map<char,set<NFANode*>> getNextMap()
    {
        return nextMap;
    }
    int getState()
    {
        return state;
    }
};

class NFA
{
public:
    map<char,set<int>> nfaTable[1000];
    vector< set<int> > dfaStates;
    set<int> endStates;
    set<char> charSet;
    stack<char> operatorStack;
    stack<NFANode*> nfaMapStack;
    set<int> outedStateSet;
    int maxState=0;
    NFA()
    {
        nfaTable[0][EPSILON].insert(1);
        nfaTable[1]['a'].insert(1);
        nfaTable[1]['b'].insert(1);
        nfaTable[1][EPSILON].insert(2);
        nfaTable[2]['b'].insert(3);
        nfaTable[3]['b'].insert(4);
        nfaTable[4]['>'].insert(5);
        nfaTable[4]['<'].insert(5);
        nfaTable[4]['!'].insert(6);
        nfaTable[4]['='].insert(6);
        nfaTable[5]['='].insert(7);
        nfaTable[5][EPSILON].insert(7);
        nfaTable[6]['='].insert(7);
        nfaTable[7]['1'].insert(8);
        endStates.insert(8);
        charSet={'a','b','<','>','=','!','1'};
    }
    NFA(const string& regex)
    {
        for(int i=0;i<regex.length();i++)
        {
            char c=regex[i];
            if(c=='('||c=='+'||c=='|')
            {
                operatorStack.push(c);
            }
            else if(c=='*')
            {
                NFANode* node=nfaMapStack.top();
                NFANode* start=nfaMapStack.top();
                nfaMapStack.pop();
                auto* startNode=new NFANode(++maxState);
                auto* endNode=new NFANode(++maxState);
                startNode->addNext(EPSILON,start);
                startNode->addNext(EPSILON,endNode);
                node=node->getEnd();
                node->addNext(EPSILON,start);
                node->addNext(EPSILON,endNode);
                nfaMapStack.push(startNode);
                if(i+1<regex.length()&&regex[i+1]!=')'&&regex[i+1]!='|'&&regex[i+1]!='*')
                {
                    operatorStack.push('~');
                }
            }
            else if(c==')')
            {
                char op=operatorStack.top();
                operatorStack.pop();
                while(op!='(')
                {
                    calculate(op);
                    op=operatorStack.top();
                    operatorStack.pop();
                }
                if(i+1<regex.length()&&regex[i+1]!=')'&&regex[i+1]!='|'&&regex[i+1]!='*')
                {
                    operatorStack.push('~');
                }
            }
            else
            {
                if(!charSet.count(c)&&c!=EPSILON)
                {
                    charSet.insert(c);
                }
                auto* startNode=new NFANode(++maxState);
                auto* endNode=new NFANode(++maxState);
                startNode->addNext(c,endNode);
                nfaMapStack.push(startNode);
                if(i+1<regex.length()&&regex[i+1]!=')'&&regex[i+1]!='|'&&regex[i+1]!='*')
                {
                    operatorStack.push('~');
                }
            }
        }
        while(!operatorStack.empty())
        {
            char op=operatorStack.top();
            if(op=='(')
            {
                cout<<"regex error: cause by '('"<<endl;
                return;
            }
            operatorStack.pop();
            calculate(op);
        }
        if(nfaMapStack.size()!=1)
        {
            cout<<"regex error:internal error"<<endl;
            return;
        }
        NFANode* nfaNode=nfaMapStack.top();
        endStates.insert(nfaNode->getEnd()->getState());
        nfaTable[0][EPSILON].insert(nfaNode->getState());
    }
    void output()
    {
        list<NFANode*> outputList;
        outputList.push_back(nfaMapStack.top());
        outedStateSet.insert(nfaMapStack.top()->getState());
        while(!outputList.empty())
        {
            NFANode* nfaNode=outputList.front();
            outputList.pop_front();
            cout<<setw(5)<<nfaNode->getState();
            for(const auto& itr:nfaNode->getNextMap())
            {
                for(auto it:itr.second)
                {
                    /*输出过程中将链表形式的nfa表转换为状态转换表*/
                    nfaTable[nfaNode->getState()][itr.first].insert(it->getState());
                    if(itr.first==EPSILON)
                    {
                        cout<<"EPSILON"<<"->"<<it->getState()<<" ";
                    }
                    else
                    {
                        cout<<"'"<<itr.first<<"'->"<<it->getState()<<" ";
                    }
                }
            }
            if(nfaNode->isEnd())
            {
                cout<<"(END)";
            }
            cout<<endl;
            for(const auto& itr:nfaNode->getNextMap())
            {
                for(auto it:itr.second)
                {
                    if(!outedStateSet.count(it->getState()))
                    {
                        outputList.push_back(it);
                        outedStateSet.insert(it->getState());
                    }
                }
            }
        }
        cout<<endl;
    }
    void calculate(char op)
    {
        if(op=='~')
        {
            NFANode* secondNode=nfaMapStack.top();
            nfaMapStack.pop();
            NFANode* prevNode=nfaMapStack.top();
            prevNode=prevNode->getEnd();
            prevNode->setNext(secondNode->getNextMap());
        }
        if(op=='|'||op=='+')
        {
            auto* startNode=new NFANode(++maxState);
            auto* endNode=new NFANode(++maxState);
            NFANode* secondNode=nfaMapStack.top();
            nfaMapStack.pop();
            NFANode* prevNode=nfaMapStack.top();
            nfaMapStack.pop();
            startNode->addNext(EPSILON,prevNode);
            startNode->addNext(EPSILON,secondNode);
            prevNode=prevNode->getEnd();
            secondNode=secondNode->getEnd();
            secondNode->addNext(EPSILON,endNode);
            prevNode->addNext(EPSILON,endNode);
            nfaMapStack.push(startNode);
        }
    }
    set<int> epsilonExtend(const set<int>& states)
    {
        set<int> stateSet;
        stateSet.insert(states.begin(), states.end());
        for(int i:states)
        {
            if(!nfaTable[i][EPSILON].empty())
            {
                set<int> result=epsilonExtend(nfaTable[i][EPSILON]);
                stateSet.insert(result.begin(), result.end());
            }
        }
        return stateSet;
    }
    set<int> move(const set<int>& stateSet,char c)
    {
        set<int> result;
        for(int state : stateSet)
        {
            if(!nfaTable[state][c].empty())
            {
                result.insert(nfaTable[state][c].begin(), nfaTable[state][c].end());
            }
        }
        return result;
    }
    int getNextState(const set<int>& state,char c)
    {
        set<int> result=move(state,c);
        if(result.empty())
        {
            return -1;
        }
        set<int> newState;
        set<int> epsilonExtendedState=epsilonExtend(result);
        newState.insert(epsilonExtendedState.begin(), epsilonExtendedState.end());
        int i;
        for(i=0;i<dfaStates.size();i++)
        {
            if(newState==dfaStates[i])
            {
                return i;
            }
        }
        dfaStates.push_back(newState);
        return i;
    }
};

class DFA
{
public:
    map<char,int> dfaTable[1000];
    set<int> endStates;
    set<char> charSet;
    DFA(NFA& nfa)
    {
        this->charSet=nfa.charSet;
        nfa.dfaStates.push_back(nfa.epsilonExtend({0}));
        int listLength=1;
        int current=0;
        int maxState=0;
        int list[10000];
        list[0]=0;
        while(listLength!=0)
        {
            map<char,int> nextState;
            for(char c:charSet)
            {
                nextState[c]=nfa.getNextState(nfa.dfaStates[list[current]],c);
                this->dfaTable[list[current]][c]=nextState[c];
                if(nextState[c]==-1)
                {
                    continue;
                }
                for(int i:nfa.dfaStates[nextState[c]])
                {
                    if(nfa.endStates.count(i))
                    {
                        endStates.insert(nextState[c]);
                        break;
                    }
                }
                if(nextState[c]>maxState)
                {
                    maxState=nextState[c];
                    list[current+listLength]=nextState[c];
                    listLength++;
                }
            }
            listLength--;
            current++;
        }
    }
    int move(int currentState,char c)
    {
        if(currentState==-1)
        {
            return -1;
        }
        if(!charSet.count(c))
        {
            return -1;
        }
        return dfaTable[currentState][c];
    }
    void output()
    {
        int state=-1;
        for(const map<char,int>& dfaLine:dfaTable)
        {
            state++;
            if(dfaLine.count(EPSILON))
            {
                continue;
            }
            if(dfaLine.empty())
            {
                break;
            }
            cout<<setw(5)<<state;
            for(auto & itr : dfaLine)
            {
                if(itr.second==-1)
                {
                    continue;
                }
                cout<<"'"<<itr.first<<"'->"<<itr.second<<" ";
            }
            cout<<endl;
        }
        cout<<endl;
    }
    void outputEndStates()
    {
        cout<<"endStates:";
        for(auto itr:endStates)
        {
            cout<<itr<<" ";
        }
        cout<<endl;
        cout<<endl;
    }
    void outputCharSet()
    {
        for(auto itr:charSet)
        {
            cout<<itr<<" ";
        }
        cout<<endl;
        cout<<endl;
    }
    void simplify()
    {
        for(int i=0;i<100;i++)
        {
            if(dfaTable[i].empty())
            {
                break;
            }
            if(dfaTable[i].count(EPSILON))
            {
                continue;
            }
            for(int j=0;j<100;j++)
            {
                if(dfaTable[j].empty())
                {
                    break;
                }
                if(dfaTable[j].count(EPSILON)||i==j)
                {
                    continue;
                }
                int repeat=true;
                for(auto k:dfaTable[i])
                {
                    if(dfaTable[j][k.first]!=k.second)
                    {
                        repeat=false;
                        break;
                    }
                }
                if(repeat)
                {
                    dfaTable[j][EPSILON]=-1;
                    for(map<char,int>& dfaLine:dfaTable)
                    {
                        for(auto & itr : dfaLine)
                        {
                            if(itr.second==j)
                            {
                                itr.second=i;
                            }
                        }
                    }
                }
            }
        }
    }
    void match(const string& temp)
    {
        int currentState=0;
        for(char c:temp)
        {
            if(c==';')
            {
                break;
            }
            if(c==' ')
            {
                continue;
            }
            currentState=move(currentState,c);
        }
        if(endStates.count(currentState))
        {
            cout<<temp<<"--yes"<<endl;
        }
        else
        {
            cout<<temp<<"--no"<<endl;
        }
    }
};

int main()
{
    string regex="(a|b)*bb(((>|<)(=|`))|((!|=)=))1";
    cout<<"please input regex(use '`' to represent EPSILON):";
    cin>>regex;
    cout.setf(ios::left);
    NFA nfa=NFA(regex);
    cout<<"NFA:"<<endl;
    nfa.output();
    DFA dfa=DFA(nfa);
    cout<<"DFA:"<<endl;
    dfa.output();
    dfa.simplify();
    cout<<"simplified DFA:"<<endl;
    dfa.output();
    dfa.outputEndStates();
    cout<<"supported char:"<<endl;
    dfa.outputCharSet();

    while(true)
    {
        cout<<"please input string to test(input 'exit' to quit):";
        string temp;
        cin>>temp;
        if(temp=="exit")
        {
            break;
        }
        dfa.match(temp);
    }
    return 0;
}
