#include <iostream>
#include <string.h>
#include <cassert>
#include <ctime>
#include <cstdlib>
#include <conio.h>
#define MAXSCORE 10000
#define DEBUG
#ifdef DEBUG
#define DPRINT(...) fprintf(stderr, __VA_ARGS__);
#define dprint() print()
#else  /* DEBUG */
#define DPRINT(...)
#endif /* DEBUG */
#define INF 1000000
using namespace std;

struct state{
    char grid[9];
    int score;
    int next_state_cnt;
    state *next_states[9];

    state(){
        next_state_cnt = 0;
        fill(next_states, next_states + 9, nullptr);
        memset(grid,'-',9);
        score = 0;
    }

    state(const state& rhs){
        next_state_cnt = 0;
        fill(next_states, next_states + 9, nullptr);
        memcpy(grid, rhs.grid, 9);
        score = 0;
    }

    ~state(){
        for(auto s : next_states){
            delete s;
        }
    }

    void operator=(const state& rhs){
        memcpy(grid, rhs.grid, 9);
    }

    bool operator==(const state& rhs) const{
        for(int i = 0; i < 9; ++i){
            if(grid[i] != rhs.grid[i])
                return false;
        }
        return true;
    }

    char who_won(){
        int sum;
        // check rows
        for(int i = 0; i <= 6; i+=3){
            sum = 0;
            for(int j = i; j < i+3; ++j){
                sum += grid[j];
            }
            if(sum == 3*'O') return 1;
            else if(sum == 3*'X') return -1;
        }
        // check columns
        for(int i = 0; i < 3; i++){
            sum = 0;
            for(int j = i; j <= i+6; j += 3){
                sum += grid[j];
            }
            if(sum == 3*'O') return 1;
            else if(sum == 3*'X') return -1;
        }
        // check diagonals
        for(int i = 0; i < 3; i+=2){
            sum = 0;
            for(int j = i; j <= i+(4-i)*2; j += 4-i){
                sum += grid[j];
            }
            if(sum == 3*'O') return 1;
            else if(sum == 3*'X') return -1;
        }
        return 0;
    }
    bool is_full(){
        for(char x : grid){
            if(x == '-')
                return false;
        }
        return true;
    }
    void print(){
        for(int i = 0; i < 9; ++i){
            cout << grid[i] << ' ';
            if(i % 3 == 2) cout << '\n';
        }
        cout << '\n';
    }
    #ifndef DEBUG
    void dprint(){
    }
    #endif // DEBUG
};

void generate_tree(state* current, bool turn){
    int res;
    assert(current);
    if(current->is_full()){
        current->score = current->who_won() * MAXSCORE;
        return;
    }
    if(res = current->who_won()){
        current->score = res * MAXSCORE;
        return;
    }

    for(int i = 0; i < 9; ++i){
        if(current->grid[i] == '-'){
            state* next_state = new state(*current);
            next_state->grid[i] = (turn ? 'X' : 'O');
            generate_tree(next_state, !turn);
            current->next_states[i] = next_state;
            current->next_state_cnt++;
        }
    }
    int sum = 0, win_ratio = 1;
    for(int i = 0; i < 9; ++i){
        if(current->next_states[i] != nullptr){
            if(current->next_states[i]->who_won())
                win_ratio++;
            sum += current->next_states[i]->score;
        }
    }
    current->score = (sum * win_ratio) / current->next_state_cnt;
}

void player_move(state** current){
    int x;
    cout << "Your move "; cin >> x;
    while(0 <= x and x < 9 and (*current)->next_states[x] == nullptr){
        cout << "Invalid move. Try again "; cin >> x;
    }
    *current = (*current)->next_states[x];
}

void pc_move(state** current, bool player_first){
    int pc_score = (player_first ? INF : -INF);
    int pc_pos = 0;
    for(int i = 0; i < 9; ++i){
        if((*current)->next_states[i] == nullptr) continue;

        DPRINT("%d\n",(*current)->next_states[i]->score);
        (*current)->next_states[i]->dprint();

        if(player_first){
            if((*current)->next_states[i]->score < pc_score){
                pc_score = (*current)->next_states[i]->score;
                pc_pos = i;
            }
        }
        else {
            if((*current)->next_states[i]->score > pc_score){
                pc_score = (*current)->next_states[i]->score;
                pc_pos = i;
            }
        }
    }
    cout << "PC move " << pc_pos << endl;
    *current = (*current)->next_states[pc_pos];
}

int main(){
    state* init = new state;
    srand(time(NULL));
    generate_tree(init,0);

    cout << "[Move guide table]\n";
    for(int i = 0; i < 9; ++i){
        cout << i << ' ';
        if(i % 3 == 2) cout << '\n';
    }
    cout.put('\n');

    state* current = init;
    bool player_first = rand()%2;

    current->print();
    if(player_first){
        //player first
        cout << "You are going first!\n";
        while(!current->is_full() and !current->who_won()){
            player_move(&current);
            current->print();
            if(!current->is_full() and !current->who_won()){
                pc_move(&current, player_first);
                current->print();
            }
        }
    }
    else {
        // PC first
        cout << "PC is going first!\n";
        while(!current->is_full() and !current->who_won()){
            pc_move(&current, player_first);
            current->print();
            if(!current->is_full() and !current->who_won()){
                player_move(&current);
                current->print();
            }
        }
    }
    DPRINT("%d\n", current->score);
    current->dprint();
    switch (current->who_won()){
        case 1 :
            printf("%s won!\n", player_first ? "Player" : "PC");
            break;
        case -1 :
            printf("%s won!\n", player_first ? "PC" : "Player");
            break;
        default :
            cout << "Tie!\n" << endl;
            break;
    }
    delete init;
    cout << "Press any key to continue... ";
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
    return 0;
}
