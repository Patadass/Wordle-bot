#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>
#include <random>

using namespace std;

//trying commit

vector<string> getNewLetters(vector<string>& letters){
    string letter;
    for(int i = 0;i < letters.size();i++){
        cout<<letters[i];
    }
    cout<<endl;
    cin>>letter;
    for(int i = 0;i < letter.size();i++){
        if(letter[i] >= 'a' && letter[i] <= 'z'){
            letter[i] -=' ';
        }
        letters[i] += letter[i];
    }
    return letters;
}

string toLowerCase(string text){
    for(int i = 0;i < text.size();i++){
        if(text[i] >= 'A' && text[i] <= 'Z'){
            text[i] += ' ';
        }
    }
    return text;
}

int isLineAllowed(string line){
    if(line.size() != 5){
        return 1;
    }
    for(int i = 0; i < line.size(); i++){
        if(line[i] < 'a' || line[i] > 'z'){
            return 2;
        }
    }
    fstream file;
    file.open("dictionary/wordle-La.txt");
    if(!file.is_open()){
        cout<<"CAN NOT CHECK DICTIONARY WORD MIGHT NOT BE ALLOWED"<<endl;
        return 0;
    }
    string word;
    bool isInDictionary = false;
    while (getline(file,word)){
        if(word == line){
            isInDictionary = true;
            break;
        }
    }
    file.close();
    if(!isInDictionary){
        return 3;
    }
    return 0;
}

void wordle_bot(vector<string>& listOfWords,string line,const string& wantSave){
    /*todo:make algorithm better
     * 1.it gets confused when there is a yellow and grey of the same color it still works but not efficiently
     * 2.maybe if there are 4 greens it should instead of continuing to guess with those 4 letters-
     * -it should guess once with the remaining letters so that it eliminates more letters and makes the word easier to guess-
     * -instead of just going through the list and trying to guess every word with those 4 letters*/

    fstream file;
    bool can_skip = false;
    bool is_over = false;

    int counter_for_over = 0,size_of_a_word = 5;

    string letter;

    vector<string> saveWord;
    vector<string> saveLetterColor;

    vector<char> not_letters;
    vector<int> not_letters_pos;

    vector<string> letters = {"a","l","e","r","t"};
    letters[0] = line[0];
    letters[1] = line[1];
    letters[2] = line[2];
    letters[3] = line[3];
    letters[4] = line[4];

    saveWord.emplace_back(line);

    letters = getNewLetters(letters);
    string letterColor;
    for(int j = 0;j < letters.size();j++){
        letter = letters[j];
        letterColor += letter[1];
        if(j != 0)saveLetterColor.pop_back();
        saveLetterColor.push_back(letterColor);
    }
    for(int j = 0;j < size_of_a_word;j++){
        line = letters[j];
        for(int k = 0;k < size_of_a_word;k++){
            letter = letters[k];
            if(letter[0] == line[0]){
                if(letter[1] == 'Y' || letter[1] == 'G'){
                    can_skip = true;
                }
            }
        }
        if(can_skip)continue;
        if(line[1] == 'N'){
            not_letters.push_back(line[0]);
            not_letters_pos.push_back(j);
        }
    }

    for(int i = 0;i < listOfWords.size();i++){
        can_skip = false;
        line = listOfWords[i];
        listOfWords[i] = " ";
        if(line == " "){
            continue;
        }
        for(int j = 0;j < line.size();j++){
            for(int k = 0;k < not_letters.size();k++){
                if(line[j] == not_letters[k]){
                    can_skip = true;
                    break;
                }
            }
            if(can_skip)break;
        }
        if(can_skip)continue;
        for(int j = 0;j < size_of_a_word;j++){
            letter = letters[j];
            if(letter[1] == 'N')continue;
            if(letter[1] == 'G'){
                if(line[j] != letter[0]){
                    can_skip = true;
                    break;
                }
            }
            if(letter[1] == 'Y'){//TODO: check if there are 2 yellows of same letter so that it doesnt give words with yellow in that possiton
                bool has_letter = false;
                for(int k = 0;k < size_of_a_word;k++){
                    if(k == j)continue;
                    if(line[k] == letter[0]){
                        has_letter = true;
                        break;
                    }
                }
                if(!has_letter){
                    can_skip = true;
                    break;
                }
            }
        }
        if(can_skip)continue;
        counter_for_over++;
        if(counter_for_over > 5){
            counter_for_over--;
            break;
        }
        saveWord.push_back(line);
        letters[0] = line[0];
        letters[1] = line[1];
        letters[2] = line[2];
        letters[3] = line[3];
        letters[4] = line[4];
        letters = getNewLetters(letters);
        is_over = true;
        letterColor = "";
        for(int j = 0;j < letters.size();j++){
            letter = letters[j];
            letterColor += letter[1];
            if(j != 0)saveLetterColor.pop_back();
            saveLetterColor.push_back(letterColor);
            if(letter[1] != 'G'){
                is_over = false;
            }
        }
        if(is_over)break;
        i = 0;
        for(int j = 0;j < size_of_a_word;j++){
            can_skip = false;
            line = letters[j];
            for(int k = 0;k < size_of_a_word;k++){
                letter = letters[k];
                if(letter[0] == line[0]){
                    if(letter[1] == 'Y' || letter[1] == 'G'){
                        can_skip = true;
                    }
                }
            }
            if(can_skip)continue;
            if(line[1] == 'N'){
                not_letters.push_back(line[0]);
                not_letters_pos.push_back(j);
            }
        }
    }
    if(wantSave == "yes"){
        auto start = std::chrono::system_clock::now();
        auto end = std::chrono::system_clock::now();
        std::time_t end_time = std::chrono::system_clock::to_time_t(end);
        //ctime(&end_time)
        file.open("saves/all_attempts.txt", ios::app);
        if(!file.is_open()){
            cout<<"COULD NOT OPEN SAVE FILE"<<endl;
            cout<<"ATTEMPT HAS NOT BEEN SAVED"<<endl;
            return;
        }
        if(is_over){
            file<<"------------------WON-----------------------"<<endl;
            file<<"GUESSED THE WORD "<<saveWord.back()<<endl;
            file<<"IN "<<++counter_for_over<<" GUESSES"<<endl;
        }else{
            file<<"------------------LOST-----------------------"<<endl;
        }
        file<<"TIME OF GUESSING:"<<ctime(&end_time)<<endl;
        for(int i = 0;i < saveWord.size();i++){
            file<<saveWord[i]<<endl;
            file<<saveLetterColor[i]<<endl;
            file<<endl;
        }
        file<<"--------------------------------------------"<<endl;
        file.close();
        cout<<"ATTEMPT SAVED SUCCESSFULLY"<<endl;
    }
}

int main() {
    string line,wantSave,wantRandom;
    cout<<"Save attempt (yes) (no):";
    cin>>wantSave;
    cout<<"Start with random word (yes) (no):";
    cin>>wantRandom;

    wantSave = toLowerCase(wantSave);
    wantRandom = toLowerCase(wantRandom);
    string wantOwnWord;
    if(wantRandom == "no"){
        cout<<"Start with your own word (yes) (no):";
        cin>>wantOwnWord;
        wantOwnWord = toLowerCase(wantOwnWord);
    }

    fstream file;

    vector<string> listOfWords;

    file.open("dictionary/wordle-La.txt");
    if(!file.is_open()){
        cout<<"COULDN'T OPEN wordle-La.txt"<<endl;
    }
    for(int i = 0; getline(file,line);i++){
        listOfWords.push_back(line);
    }
    file.close();

    /*todo:random stopped working for some reason
     * update:random now only works in clion compiler*/
    if(wantRandom == "yes"){
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<int> distribution(1,listOfWords.size()-1);
        int dice_roll = distribution(gen);
        line = listOfWords[dice_roll];
    }else{
        if(wantOwnWord == "yes"){
            cout<<"Word to start with:";
            cin>>line;
            line = toLowerCase(line);
            while (isLineAllowed(line) != 0){
                if(isLineAllowed(line) == 1){
                    cout<<"WORD MUST BE EXACTLY 5 LETTERS LONG:";
                    cin>>line;
                }
                if(isLineAllowed(line) == 2){
                    cout<<"WORD MUST CONTAIN ONLY LETERS (a-z)";
                    cin>>line;
                }
                if(isLineAllowed(line) == 3){
                    cout<<"WORD NOT RECOGNIZED BY WORDLE PLEASE INPUT DIFFERENT WORD:";
                    cin>>line;
                }
            }
        }else{
            line = "alert";
        }
    }


    wordle_bot(listOfWords,line,wantSave);

    return 0;
}
