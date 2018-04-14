#include <cctype>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
typedef int Number;

enum Token_value : char {
  NAME, NUMBER, END,
  PLUS = '+', MINUS = '-', MUL = '*', DIV = '/',
  PRINT = ';', ASSIGN = '=', LP = '(', RP = ')', FUNC = 'f'
};

enum Number_value : char {
  NUM0 = '0', NUM1 = '1', NUM2 = '2',
  NUM3 = '3', NUM4 = '4', NUM5 = '5',
  NUM6 = '6', NUM7 = '7', NUM8 = '8',
  NUM9 = '9',
};

Token_value curr_tok = PRINT;        // Хранит последний возврат функции get_token().
Number number_value;                 // Хранит целый литерал или литерал с плавающей запятой.
std::string string_value;            // Хранит имя.
std::map<std::string, Number> table; // Таблица имён.
int no_of_errors;                    // Хранит количество встречаемых ошибок.
//TODO class ERROR&&ERROR_PRINTER

Number expr(std::istream *, bool);    // Обязательное объявление.

/****************************************************************************/

void print(Token_value value);
// Функция error() имеет тривиальный характер: инкрементирует счётчик ошибок.
Number error(const std::string &error_message) {
  ++no_of_errors;
  std::cerr << "error: " << error_message << std::endl;
  return 1;
  //TODO throw err_not_implement;
}

Token_value get_token(std::istream *input) {
  char ch;

  do {    // Пропустить все пробельные символы кроме '\n'.
    if (!input->get(ch)) {
      return curr_tok = END; // Завершить работу калькулятора.
    }
  } while (ch != '\n' && isspace(ch));

  switch (ch) {
    case 0: // При вводе символа конца файла, завершить работу калькулятора.
      return curr_tok = END;
    case PRINT:
    case '\n':return curr_tok = PRINT;
      {//operation
        case PLUS:
        case FUNC:
        case ASSIGN:return curr_tok = Token_value(ch); // Приведение к целому и возврат.
      }
      {//number
        case NUM0:
        case NUM1:
        case NUM2:
        case NUM3:
        case NUM4:
        case NUM5:
        case NUM6:
        case NUM7:
        case NUM8:
        case NUM9:input->putback(ch); // Положить назад в поток...
        *input >> number_value; // И считать всю лексему.
        return curr_tok = NUMBER;
      }
    default:
      if (isalpha(ch)) {
        string_value = ch;
        while (input->get(ch) && isalnum(ch)) {
          string_value.push_back(ch);
        }
        input->putback(ch);
        return curr_tok = NAME;
      }
      error("Bad Token");
      return curr_tok = PRINT;
  }
}

/* Каждая функция синтаксического анализа принимает аргумент типа bool
 * указывающий, должна ли функция вызывать get_token() для получения
 * очередной лексемы. */

// prim() - обрабатывает первичные выражения.
Number prim(std::istream *input, bool get) {
  if (get) {
    get_token(input);
  }

  switch (curr_tok) {
    case NUMBER: {
      Number v = number_value;
      get_token(input);
      return v;
    }
    case NAME: {
      Number &v = table[string_value];
      if (get_token(input) == ASSIGN) {
        v = expr(input, true);
      }
      return v;
    }

    case LP: {
      Number e = expr(input, true);
      if (curr_tok != RP) {
        return error("')' expected");
      }
      get_token(input);
      return e;
    }
    case FUNC://TODO throw not-implentetd
    default:return error("primary expected");
  }
}

// term() - умножение и деление.
Number term(std::istream *input, bool get) {
  Number left = prim(input, get);

  for (;;) {
    switch (curr_tok) {
      case MUL:left *= prim(input, true);
        break;
      case DIV:
        if (double d = prim(input, true)) {
          left /= d;
          break;
        }
        return error("Divide by 0");
      default:return left;
    }
  }
}

// expr() - сложение и вычитание.
Number expr(std::istream *input, bool get) {
  Number left = term(input, get);

  for (;;) {
    switch (curr_tok) {
      case PLUS:
        print(PLUS);
        left += term(input, true);
        break;
      default:return left;
    }
  }
}
void print(Token_value value) {
  std::cout << value;
}

int main(int argc, char *argv[]) {
  std::istream *input = nullptr; // Указатель на поток.

  switch (argc) {
    case 1:input = &std::cin;
      break;
    case 2:input = new std::istringstream(argv[1]);
      break;
    default:error("Too many arguments");
      return 1;
  }

  while (*input) {
    get_token(input);
    if (curr_tok == END) {
      break;
    }

    // Снимает ответственность expr() за обработку пустых выражений.
    if (curr_tok == PRINT) {
      continue;
    }
    //expr()->func()->prim()->expr() ....
    std::cout << expr(input, false) << std::endl;
  }

  if (input != &std::cin) {//free memory if we need to.
    delete input;
  }

  return no_of_errors;//TODO print errors
}