#include "AT_parser.h"
#include "stm32f0xx.h"
#include "strings.h"

// wskaznik na tablice ze strukturami komend AT
const t_cmd * AT_command_array;

// ilosc elementow w tablicy komend
int AT_commands_number = 0;

//! funkcja rejestrujaca tablice z komendami AT
//! podajemy wskaznik na tablice oraz jej wielkosc
void AT_register_AT_commands_table(const t_cmd *wsk, uint8_t ilosc_parametrow)
{
    AT_command_array = wsk;
    AT_commands_number = ilosc_parametrow;
}

//! glowna funkcja dekodujaca komendy AT
//! jako parametr przyjmuje wskaznik na bufor z odebranym napisem
void AT_commands_decode(char* data)
{
    // statici - zeby wrzucic je na stale do RAMu
    // tablica z parametrami
    static char* params[_MAX_PARAMETERS];

    // ilosc znalezionych parametrow
    static int params_cnt;

	// test naglowka komendy - musi to byc "AT+"
	if(data[0] == 'A' && data[1] == 'T' && data[2] == '+')
	{
		// przesuniecie wskaznika
		data += 3;

        // wskaznik tymczasowy dla operacji na lancuchu
        char * temp_wsk = 0;

        // musimy zamienic znak rownosci na 0 - bo nie porownamy inaczej napisow
        temp_wsk = strchr(data,'=');
        if(temp_wsk)
        {
            *temp_wsk = 0;
        }

		// petla po wszystkich komendach - jesli nie mamy w sobie odpowiedniej komendy - wykonamy wszystkie iteracje
		// w przypadku znalezienia komendy - opuszczamy petle
		int i=0; //Utworzenie zmiennej i, aby była widoczna na zewnątrz
		for( i = 0; i < AT_commands_number; i++)
		{
		    // porownanie odebranego napisu i komend z tablicy
			if( strcmp(AT_command_array[i].cmd, data ) == 0 )
			{
				//  sprawdzmy czy callback jest podpiety - jesli nie nie ma sensu dekodowac parametrow
				if( AT_command_array[i].callback_function )
				{
					params_cnt = 0; // zerowanie licznika parametrow

					// przywrocenie znaku rownosci:
					if(temp_wsk)
					{
						*temp_wsk = '=';
					}

					// sprawdzamy, czy mamy znak "=" po komendzie AT:
					if( data[strlen(AT_command_array[i].cmd)] == '=' )
					{
						// przesunmy sobie wskaznik
						data += strlen(AT_command_array[i].cmd) + 1;

						// ustawiamy pierwszy parametr (jesli wystepuje)
						if(data[0] != 0)
						{
							params[0] = strtok(data, ",");

							// jesli pierwszy parametr ma jakas dlugosc:
							if(params[0] != 0)
							{
								params_cnt++;
								// dokonujemy rozdzielania komend
								for(int i = 1; i < _MAX_PARAMETERS; i++)
								{
									params[i] = strtok(NULL, ",");

									if(params[i] == NULL)
									{
										break;
									}
									params_cnt++;
								}
							}
						}
					}
					// wywolanie funkcji
					AT_command_array[i].callback_function(params, params_cnt);
				}
				// opuszczenie funkcji parsujacej - nie skanujemy dalszych komend
				break;
			}
		}
		
		// bledna komenda (taka której nazwa zaczyna się od AT+), ale nie ma jej w tabeli AT_cmd_array[]  - mozna dodac jakas defaultowy event w przyszlosci
		// np do przeslania ramki ERROR
		
	
	}
	else
    {
        // bledna komenda (taka której nazwa nie zaczyna się od AT+) - mozna dodac jakas defaultowy event w przyszlosci
	// np do przeslania ramki ERROR
    }
}
