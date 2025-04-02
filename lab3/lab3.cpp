// Дудолин Д.А. УИС-312
// Разделяемая переменная: R(str)
// Переменная-дублер: Rcontr 
// Операция 1: Добавить символ справа
// Операция 2: при нажатии клавиш 1-9 стирание последних N символов
// Вариант 6б 
#include <windows.h> 
#include "conio.h"
#include <iostream> 
#include <time.h> 
#include<string>



using namespace std;
LONG volatile keyLocked = 0;
HANDLE hThread; // дескриптор создаваемого потока 
DWORD IDThread;// идентификатор потока 



string R = "ABCDE"; // разделяемая глобальная переменная 
clock_t wtime; // время задержки (мс) 


//объявление функций для создания потоков
DWORD WINAPI addSymbol(struct log1* p);
DWORD WINAPI deleteSymbol(struct log1* p);

// функция задержки 
// параметр - время задержки в мс 
void waiting(clock_t dt);



struct log1 {
	int nom; // порядковый номер операции 
	int key; // код нажатой клавиши 
	DWORD ID; // идентификатор созданного потока 
	clock_t timeInThread;
	clock_t t1; // время начала операции 
	clock_t t2; // время завершения операции 
	string R; // результат операции в дочернем потоке 
	// (разделяемая переменная) 
	string Rc; // результат операции главном потоке 
	// (переменная-дублер) 
	int znah;
}
mylog[1000]; // не более 1000 строк в журнале 
//=================================================



int main(int argc, char* argv[]) // главный поток 
{
	wtime = 1000;
	setlocale(LC_ALL, "Russian");
	SetConsoleCP(1251);
	cout << "Время задержки (ms): ";
	cin >> wtime;
	int i = 0; // счетчик операций 
	int j = 1; // счетчик вывода строк журнала 
	int key = 1; // код нажатой клавиши 
	int flag = 1; // признак завершения программы 
	string Rcontr; // переменная-дублер 
	mylog[1].t2 = 0; // признак того, что поток не завершен 
	Rcontr = R;
	// повторять пока flag !=0 или i<1000 
	cout << "Символы - добавить символ в конец строки" << endl << "1-9 - удалить с конца" << endl;
	while (flag || i >= 1000)
	{
		//Прием кода нажатой клавиши 
		if (_kbhit() != 0)
		{
			key = _getch();
			if (key >= 49 && key <= 57)
				if(!Rcontr.empty())
					for (size_t i = 0; i < key - 48 && Rcontr.size() >= 1; i++)
					{
						Rcontr.pop_back();
					}
			if(!(key >= 49 && key <= 57))
			{
				Rcontr += static_cast<char>(key);
			}

		i = ++i;
		// формируем строку журнала 
		mylog[i].nom = i;
		mylog[i].key = key;
		mylog[i].t2 = 0;
		mylog[i].R = "";
		mylog[i].Rc = Rcontr;
		mylog[i].t1 = clock();
		// создание потока без немедленного запуска 	
		
		if (key >= 49 && key <= 57) 
		{
			hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)deleteSymbol, &mylog[i], CREATE_SUSPENDED, &IDThread);
		}
		if (!(key >= 49 && key <= 57))
		{
			hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)addSymbol, &mylog[i], CREATE_SUSPENDED, &IDThread);
		}
		if (hThread == NULL)
		{
			cout << "Ошибка при создании потока" << key << endl;
			_getch();
			return 0;
		}
		else
		{
			// запуск потока 					
			mylog[i].ID = IDThread;
			ResumeThread(hThread);
		}
		}
		// завершение работы потока проверяется по 
		// сформированному в функции 
		// потока значению времени t2 
		if (mylog[j].t2 != 0)
		{
			// вывод j-й строки журнала 
			cout << j << ") key = " << char(mylog[j].key) << " IDThread = " << mylog[j].ID << " Время входа в крит область: " << mylog[j].timeInThread
				<< " t1 = " << mylog[j].t1 << " t2 = " << mylog[j].t2 << " Значение входа в крит секцию: " << mylog[j].znah << " Результат(дочерний) =  " <<
				mylog[j].R << " Результат(главный) = " << mylog[j].Rc << "" << endl;
			j = ++j; // следующая строка журнала 
		}
	} // повторять пока flag !=0 или i<1000 
	_getch();
	return 0;
}
// Функции потока: имитация "чтения", корректировки 
// и "записи" разделяемой переменной R 
// параметр - указатель на текущую строку журнала 
DWORD WINAPI deleteSymbol(struct log1* p) // функция для удаления символа при вводе цифр
{
	while (InterlockedCompareExchange(&keyLocked, 1, 0) == 1) {}
	(*p).timeInThread = clock(); // время входа в критическую область
	(*p).znah = keyLocked;
	string Rbuf;
	// чтение разделяемой переменной в буферную переменную 
	Rbuf = R;
	waiting(wtime); // задержка 
	// корректировка буферной переменной

	if (!Rbuf.empty()) {
		for (size_t i = 0; i < p->key - 48 && Rbuf.size() > 0; i++)
		{
			Rbuf.pop_back();

		}
	}


	R = Rbuf;

	(*p).R = R;
	(*p).t2 = clock();
	waiting(wtime);
	InterlockedExchange(&keyLocked, 0);
	return 0;
}

DWORD WINAPI addSymbol(struct log1* p) // функция для добавления символа при вводе букв или символов
{

	while (InterlockedCompareExchange(&keyLocked, 1, 0) == 1) {}
	(*p).timeInThread = clock(); // время входа в критическую область
	(*p).znah = keyLocked;
	string Rbuf;
	// чтение разделяемой переменной в буферную переменную 
	Rbuf = R;
	waiting(wtime); // задержка 
	// корректировка буферной переменной


	Rbuf += char((*p).key);



	R = Rbuf;

	(*p).R = R;
	(*p).t2 = clock();
	waiting(wtime);
	InterlockedExchange(&keyLocked, 0);
	return 0;
}

// функция задержки 
// параметр - время задержки в мс 
void waiting(clock_t dt) {
	clock_t tn;
	tn = clock();
	while (clock() - tn < dt);
	return;
}
