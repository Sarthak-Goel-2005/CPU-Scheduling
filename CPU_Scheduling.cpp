#include <bits/stdc++.h>
using namespace std;

int d;         // Time for running Dispatcher
ofstream Output_File("Output.txt");

class process{
    public:
    string id; // ID
    int p;     // Priority
    int at;    // Arrival Time
    int bt;    // Burst Time
    int ct;    // Completion Time
    int tat;   // Turn-Around Time
    int wt;    // Waiting Time
    
    //Constructor
    process(string ID,int Arrive,int ExecSize,int Priority){
        id=ID;
        at=Arrive;
        bt=ExecSize;
        p= Priority;
    }
};

bool comp_ID(process p1,process p2){
        if(p1.id<p2.id) return true;
        return false;
}

bool comp_AT(process p1,process p2){
        if(p1.at<p2.at) return true;
        if(p1.at==p2.at) return p1.id<p2.id;
        return false;
}

void calculate(vector<process> &v){    // To calculate TAT & WT
    for(auto &i:v){
        i.tat=i.ct-i.at;
        i.wt=i.tat-i.bt;
    }
}

void FCFS(vector<process> &v){
    int t=0;
    Output_File << "FCFS:\n";
    for(auto &i:v){
        t=max(t,i.at)+d;
        Output_File << "T" << t << ": " << i.id << "(" << i.p << ")\n";
        i.ct=t+i.bt;
        t=i.ct;
    }
    calculate(v);

    sort(v.begin(),v.end(),comp_ID);
    Output_File << "\nProcess Turnaround Time Waiting Time\n";
    for (auto &i:v) {
        Output_File<<left<<setw(8)<<i.id<<setw(16)<<i.tat<<i.wt<<"\n";
    }
}

void SPN(vector<process> &v){
    Output_File << "\nSPN:\n";
    int t=0;
    int n=v.size();
    vector<bool> completed(n,false);  // To keep track of completed processes
    int c=0;                          // No. of processes completed
    
    while(c<n){
        int idx=-1;
        int min_bt=INT_MAX;
        
        // Finding process with shortest burst time that has arrived
        for (int i=0;i<n;i++) {
            if (!completed[i] && (v[i].at<=t) && (v[i].bt<min_bt)) {
                min_bt = v[i].bt;
                idx = i;
            }
        }

        if(idx==-1){      // No processes arrived yet
            t++;
            continue;
        }
        t+=d;
        Output_File << "T" << t << ": " << v[idx].id << "(" << v[idx].p << ")\n";
        v[idx].ct = t+v[idx].bt;
        t=(v[idx].ct);
        completed[idx]=true;
        c++;
    }
    calculate(v);

    sort(v.begin(),v.end(),comp_ID);
    Output_File << "\nProcess Turnaround Time Waiting Time\n";
    for (auto &i:v) {
        Output_File<<left<<setw(8)<<i.id<<setw(16)<<i.tat<<i.wt<<"\n";
    }
}

void PP(vector<process> &v){
    Output_File << "\nPP:\n";
    int t=0;
    int n=v.size();
    string prev_process;
    vector<int> rt(n);      // Remaining burst time for each process
    for(int i=0;i<n;i++) {
        rt[i]=v[i].bt;
    }

    vector<bool> completed(n,false);
    int c=0;

    while(c<n){
        int idx=-1;
        int max_p=INT_MAX;

        for(int i=0;i<n;i++) {
            if (!completed[i] && (v[i].at<=t) && (v[i].p<max_p) && (rt[i]>0)) {
                max_p=v[i].p;
                idx=i;
            }
        }

        if(idx==-1){     // No processes has arrived yet
            t++;
            continue;
        }

        if(prev_process=="" || prev_process!=v[idx].id){          // Context switching
            t+=d;
            Output_File << "T" << t << ": " << v[idx].id << "(" << v[idx].p << ")\n";
        }

        // Process the selected task
        rt[idx]--;
        if (rt[idx]==0) {        // Process is completed
            v[idx].ct=t+1;
            completed[idx]=true;
            c++;
        }
        t++;
        prev_process=v[idx].id;
    }
    calculate(v);

    sort(v.begin(),v.end(),comp_ID);
    Output_File << "\nProcess Turnaround Time Waiting Time\n";
    for (auto &i:v) {
        Output_File<<left<<setw(8)<<i.id<<setw(16)<<i.tat<<i.wt<<"\n";
    }
}

void PRR(vector<process> &v){
    Output_File << "\nPRR:\n";
    int t=0;
    int n=v.size();
    
    queue<int> ready_queue;

    vector<int> rt(n);  // Remaining burst time for each process
    for(int i=0;i<n;i++) {
        rt[i]=v[i].bt;
    }

    vector<bool> completed(n,false);
    vector<bool> in_queue(n,false);   // To check if the process is already in queue
    int c=0;
    string prev_process;

    while(c<n) {
        // Adding processes to the queue based on arrival time
        for(int i=0;i<n;i++){
            if((v[i].at<=t) && !in_queue[i] && !completed[i]) {
                ready_queue.push(i);
                in_queue[i] = true;
            }
        }

        if(!ready_queue.empty()){
            int idx=ready_queue.front();
            ready_queue.pop();
            in_queue[idx]=false;

            if(prev_process=="" || prev_process!=v[idx].id){     // Context switching
                t+=d;
                Output_File << "T" << t << ": " << v[idx].id << "(" << v[idx].p << ")\n";
            }

            int Q;                // Time Quantum
            if(v[idx].p<=2) Q=4;
            else Q=2;
            t+=(min(Q,rt[idx]));
            rt[idx]-=(min(Q,rt[idx]));

            for(int i=0;i<n;i++){
                if((v[i].at<=t) && !in_queue[i] && !completed[i] && v[i].id!=v[idx].id) {
                    ready_queue.push(i);
                    in_queue[i] = true;
                }
            }

            if(rt[idx]==0){       // Process is completed
                v[idx].ct=t;
                completed[idx]=true;
                c++;
            } 
            else{                    // Process is not completed
                ready_queue.push(idx);
                in_queue[idx]=true;
            }
            prev_process=v[idx].id;
        } 
        else{
            t++;
        }
        
    }
    calculate(v);

    sort(v.begin(),v.end(),comp_ID);
    Output_File << "\nProcess Turnaround Time Waiting Time\n";
    for (auto &i:v) {
        Output_File<<left<<setw(8)<<i.id<<setw(16)<<i.tat<<i.wt<<"\n";
    }
}

float AVG_TAT(vector<process> &v){
    int n=v.size();
    float ans=0;
    for(auto p:v){
        ans+=p.tat;
    }
    ans/=n;
    return ans;
}

float AVG_WT(vector<process> &v){
    int n=v.size();
    float ans=0;
    for(auto p:v){
        ans+=p.wt;
    }
    ans/=n;
    return ans;
}

int main(){

    //Input from file
    string filename;
    cout<<"Enter Input file name: ";
    cin>>filename;
    ifstream Input_File(filename);
    if (!Input_File.is_open()) {
        cerr<<"Error: Could not open the file: "<<filename<<endl;
        return 1;
    }
    string line;
    string i;
    int a,b,p;
    vector<process> v;
    while (getline (Input_File, line)){
        if(line.rfind("DISP:",0)==0){
            d=stoi(line.substr(6));
        }
        else if(line.rfind("ID:",0)==0){
            i=line.substr(4);
        } 
        else if(line.rfind("Arrive:",0)==0){
            a=stoi(line.substr(8));
        } 
        else if(line.rfind("ExecSize:",0)==0){
            b=stoi(line.substr(10));
        } 
        else if(line.rfind("Priority:",0)==0){
            p=stoi(line.substr(10));
            v.push_back(process(i,a,b,p));
        }
    }
    Input_File.close();

    sort(v.begin(),v.end(),comp_AT);
    vector<process> copy(v);
    FCFS(v);
    float FCFS_avg_TAT=AVG_TAT(v);
    float FCFS_avg_WT=AVG_WT(v);
    v=copy;
    SPN(v);
    float SPN_avg_TAT=AVG_TAT(v);
    float SPN_avg_WT=AVG_WT(v);
    v=copy;
    PP(v);
    float PP_avg_TAT=AVG_TAT(v);
    float PP_avg_WT=AVG_WT(v);
    v=copy;
    PRR(v);
    float PRR_avg_TAT=AVG_TAT(v);
    float PRR_avg_WT=AVG_WT(v);

    // --- Summary ---
    Output_File<<fixed<<setprecision(2);  // Set precision to 2 decimal places
    Output_File<<"\nSummary\n";
    Output_File<<left<<setw(16)<< "Algorithm"<<setw(26)<<"Average Turnaround Time"<<"Average Waiting Time\n";
    Output_File<<left<<setw(16)<<"FCFS"<<setw(26)<<FCFS_avg_TAT<<FCFS_avg_WT<<"\n";
    Output_File<<left<<setw(16)<<"SPN"<<setw(26)<<SPN_avg_TAT<<SPN_avg_WT<< "\n";
    Output_File<<left<<setw(16)<<"PP"<<setw(26)<<PP_avg_TAT<<PP_avg_WT<< "\n";
    Output_File<<left<<setw(16)<<"PRR"<<setw(26)<<PRR_avg_TAT<<PRR_avg_WT<< "\n";
    Output_File.close();

    // Output in Terminal
    ifstream Output_File("Output.txt");
    while (getline (Output_File, line)) {
        cout<<line<<endl;
    }
    Output_File.close();
}
