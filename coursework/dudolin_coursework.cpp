// Дудолин Д.А. УИС-312 
// Вариант 39 (Поиск слов с 3 и более согласными подряд.)
#include <iostream>
#include <windows.h>
#include <string>
#include <sstream>
#include <fstream>
#include <time.h>
#include <unordered_set>
#include <cctype>

using namespace std;

HANDLE handles[15];
DWORD ids[15];
DWORD dwExitCode[15];

clock_t start_of_task, end_of_task;
clock_t start_of_funk[15];
clock_t end_of_funk[15];

struct threadData { // структура, для передачи данных в функцию потока
    string* file_name;
    string* resultText;
    string* path_to_file;
    clock_t* start_of_funk;
    clock_t* end_of_funk;
    unsigned int* count_of_words;
};

bool isConsonant(const char n) { // функция для поиска согласных букв
    char mas[42] = { 'б', 'в', 'г', 'д', 'ж', 'з', 'й', 'к', 'л', 
        'м', 'н', 'п', 'р', 'с', 'т', 'ф', 'х', 'ц', 'ч', 'ш', 'щ', 
        'Б', 'В', 'Г', 'Д', 'Ж', 'З', 'Й', 'К', 'Л', 
        'М', 'Н', 'П', 'Р', 'С', 'Т', 'Ф', 'Х', 'Ц', 'Ч', 'Ш', 'Щ'};
    for (int i = 0; i < 45; i++) {
        if (n == mas[i])
            return true;
    }
    return false;
}

std::string removePunctuation(std::string& str) { // функция для удаления знаков препинания
    str.erase(std::remove_if(str.begin(), str.end(), [](unsigned char c) {
        return std::ispunct(c);
        }), str.end());
    return str;
}

// функция для поиска трёх согласных подряд
void findTripleCharacters(const std::string& file_name, std::string& resultText, const std::string& path_to_file, clock_t& start, clock_t& end, unsigned int& count_of_words) {
    start = clock();
    count_of_words = 0;
    std::ifstream file(path_to_file + "/" + file_name); // открытие файла для чтения

    std::string word;

    /*
     Конструкция while в данном случае используется для построчного чтения данных из файла.
     Оператор >> перегружен для класса std::ifstream
     и используется для чтения данных из файла. 
     В данном случае, он считывает следующее слово 
     из файла и сохраняет его в переменную word. 
     Если операция чтения успешна, то условие while 
     считается истинным и цикл выполняется. 
     Если же операция чтения достигает конца файла или происходит ошибка, 
     условие while считается ложным и цикл завершается.
    */
    while (file >> word) {
        bool hasTripleCharacters = false;
        for (size_t i = 0; i < word.length() - 1; i++) {
            if (isConsonant(word[i]) && isConsonant(word[i + 1]) && isConsonant(word[i + 2])) {
                hasTripleCharacters = true;
                break;
            }
        }
        if (hasTripleCharacters) {
            count_of_words++;
            resultText += removePunctuation(word) + "\n";
        }
    }
    end = clock();
}


DWORD WINAPI findTripleCharactersThread(LPVOID arg) { // функция потока
    threadData* data = (threadData*)arg;
    findTripleCharacters(*(data->file_name), *(data->resultText), *(data->path_to_file), *(data->start_of_funk), *(data->end_of_funk), *(data->count_of_words));
    return 0;
}


// функция проверяет введённые имена.
bool allStringsAreDistinct(string arr[], int n) {
    unordered_set<string> hash;
    for (int i = 0; i < n; i++) {
        if (hash.find(arr[i]) != hash.end()) {
            return false;
        }
        hash.insert(arr[i]);
    }
    return true;
}

// функция проверят существует ли файл.
bool fileExists(const string filename) {
    ifstream file(filename);
    return file.good();
}

// данная функция меняет слеши в пути к файлу на обратные, чтобы не нужно было в ручную изменять скопированный путь.
string slashReplace(string file_path) {
    char replaceChar = '/';
    for (int i = 0; i < file_path.length(); i++) {
        if (file_path[i] == '\\') {
            file_path[i] = replaceChar;
        }
        return file_path;
    }
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian_Russia.1251");
    start_of_task = clock(); // Время начала работы

    int numberOfElements = argc - 4;
    int error = 0; // С помощью этой переменной мы можем поймать ошибки в ве-денных данных
    int type_of_word = atoi(argv[1]); // Переменная для выбора режима работы

    string text_name[15];
    string path_to_file = slashReplace(argv[2]); // Путь к папке с файлами
    string path_to_file_info = slashReplace(argv[3]); // Путь к папке где хранится файл лога
    string resultText[15];
    unsigned int count_of_words[15];

    threadData data[15];

    ofstream write_in_file(path_to_file_info + "/info.txt");
    write_in_file << "Журнал работы программы: " << endl; // Я каждый раз либо создаю новый файл либо перезаписываю старый

    cout << "Место чтения файла выбрано: " + path_to_file << endl;
    cout << "Место записи информации о файле: " + path_to_file_info << endl;

    for (int i = 0; i < numberOfElements; i++) {
        text_name[i] = argv[i + 4];
    }

    // проверка введённых имен.
    if (allStringsAreDistinct(text_name, numberOfElements)) {
        cout << endl << "Все введённые имена уникальны." << endl;
    }
    if (!allStringsAreDistinct(text_name, numberOfElements)) {
        cout << endl << "Есть дублирующийся имена." << endl;
        error = 1;
    }

    //проверка существования файлов.
    for (int i = 0; i < numberOfElements; i++) {
        string file_path = path_to_file + "/" + text_name[i];
        if (fileExists(file_path)) {
            cout << endl << "файл " << text_name[i] << " существует. " << endl;
        }
        else {
            cout << endl << "файл " << text_name[i] << " не существует! " << endl;
            error = 1;
            break;
        }
    }

    // проверка правильности введённого режима
    if (type_of_word != 1 && type_of_word != 2 && type_of_word != 3) {
        cout << "Введен не существующий режим!" << endl;
        error = 1;
    }

    if (error == 0) {
        for (int i = 0; i < numberOfElements; i++) {
            start_of_funk[i] = 0;
            end_of_funk[i] = 0;
            resultText[i] = "";
            data[i] = { &text_name[i], &resultText[i], &path_to_file, &start_of_funk[i], &end_of_funk[i], &count_of_words[i] };
        }
        switch (type_of_word) {
        case 1: {
                cout << "Выбран однопоточный режим. " << endl;
                for (int i = 0; i < numberOfElements; i++) {
                    start_of_funk[i] = 0;
                    end_of_funk[i] = 0;
                    findTripleCharacters(text_name[i], resultText[i], path_to_file, start_of_funk[i], end_of_funk[i], count_of_words[i]);
                    ofstream write_out_file(path_to_file_info + "/" + text_name[i] + "_out.txt");
                    write_out_file << resultText[i];
                    write_out_file.close();
                }
                break;
            }
        case 2: {
                cout << "Выбран многопоточный режим. " << endl;
                for (int i = 0; i < numberOfElements; i++) {
                    handles[i] = CreateThread(NULL, 0, findTripleCharactersThread, &data[i], 0, &ids[i]);
                }
                WaitForMultipleObjects(numberOfElements, handles, TRUE, INFINITE);
                // закрытие дескрипторов потоков
                for (int i = 0; i < numberOfElements; i++) {
                    GetExitCodeThread(handles[i], &dwExitCode[i]);
                    CloseHandle(handles[i]);
                    ofstream write_out_file(path_to_file_info + "/" + text_name[i] + "_out.txt");
                    write_out_file << resultText[i];
                    write_out_file.close();
                }
                break;
            }
        case 3: {
                cout << "Выбран многопоточный режим с приоритетом. " << endl;
                for (int i = 0; i < numberOfElements; i++) {
                    handles[i] = CreateThread(NULL, 0, findTripleCharactersThread, &data[i], CREATE_SUSPENDED, &ids[i]);
                    SetThreadPriority(handles[i], THREAD_PRIORITY_HIGHEST);
                    ResumeThread(handles[i]); // Запуск потока с новым приоритетом
                }

                WaitForMultipleObjects(numberOfElements, handles, TRUE, INFINITE);
                // закрытие дескрипторов потоков
                for (int i = 0; i < numberOfElements; i++) {
                    GetExitCodeThread(handles[i], &dwExitCode[i]);
                    CloseHandle(handles[i]);
                    ofstream write_out_file(path_to_file_info + "/" + text_name[i] + "_out.txt");
                    write_out_file << resultText[i];
                    write_out_file.close();
                }
                break;
            }
        }

        end_of_task = clock();

        for (size_t i = 0; i < numberOfElements; i++) {
            cout << endl << "===================== " << text_name[i] << " =====================" << endl;
            cout << endl << "(Количество слов с 3 и более согласными подряд в )" << i + 1 << " тек-сте: = " << count_of_words[i] << endl;
            cout << endl << "Время начала операции: " << start_of_funk[i] << endl;
            cout << endl << "Время конца операции: " << end_of_funk[i] << endl;
            cout << endl << "Время работы над данным текстом: " << (double)(end_of_funk[i] - start_of_funk[i]) / CLOCKS_PER_SEC << " секунд." << endl;

            write_in_file << endl << "===================== " << text_name[i] << " =====================" << endl;
            write_in_file << endl << "Количество слов с 3 и более согласными подряд в " << i + 1 << " тексте = " << endl << count_of_words[i] << endl;
            write_in_file << endl << "Время начала операции: " << start_of_funk[i] << endl;
            write_in_file << endl << "Время конца операции: " << end_of_funk[i] << endl;
            write_in_file << endl << "Время работы над данным текстом: " << (double)(end_of_funk[i] - start_of_funk[i]) / CLOCKS_PER_SEC << " секунд." << endl;
        }
        cout << endl << "Общее время работы: " << (double)(end_of_task - start_of_task) / CLOCKS_PER_SEC << " секунд." << endl;
        write_in_file << endl << "Общее время работы: " << (double)(end_of_task - start_of_task) / CLOCKS_PER_SEC << " секунд." << endl;

        write_in_file.close();

        if (type_of_word == 2 || type_of_word == 3) {
            for (int i = 0; i < numberOfElements; i++) {
                CloseHandle(handles[i]);
            }
        }
    }
    return 0;
}