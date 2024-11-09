# include "AIPlayer.h"
# include "Parchis.h"

const double masinf = 9999999999.0, menosinf = -9999999999.0;
const double gana = masinf - 1, pierde = menosinf + 1;
const int num_pieces = 3;
const int PROFUNDIDAD_MINIMAX = 4;  // Umbral maximo de profundidad para el metodo MiniMax
const int PROFUNDIDAD_ALFABETA = 6; // Umbral maximo de profundidad para la poda Alfa_Beta

//Funciones
void AIPlayer::thinkAleatorio(color &c_piece, int &id_piece, int &dice) const{
    // IMPLEMENTACIÓN INICIAL DEL AGENTE
    // Esta implementación realiza un movimiento aleatorio.
    // Se proporciona como ejemplo, pero se debe cambiar por una que realice un movimiento inteligente
    //como lo que se muestran al final de la función.

    // OBJETIVO: Asignar a las variables c_piece, id_piece, dice (pasadas por referencia) los valores,
    //respectivamente, de:
    // - color de ficha a mover
    // - identificador de la ficha que se va a mover
    // - valor del dado con el que se va a mover la ficha.

    // El id de mi jugador actual.
    int player = actual->getCurrentPlayerId();

    // Vector que almacenará los dados que se pueden usar para el movimiento
    vector<int> current_dices;
    // Vector que almacenará los ids de las fichas que se pueden mover para el dado elegido.
    vector<tuple<color, int>> current_pieces;

    // Se obtiene el vector de dados que se pueden usar para el movimiento
    current_dices = actual->getAvailableNormalDices(player);
    // Elijo un dado de forma aleatoria.
    dice = current_dices[rand() % current_dices.size()];

    // Se obtiene el vector de fichas que se pueden mover para el dado elegido
    current_pieces = actual->getAvailablePieces(player, dice);
    // Si tengo fichas para el dado elegido muevo una al azar.
    if (current_pieces.size() > 0)
    {
        int random_id = rand() % current_pieces.size();
        id_piece = get<1>(current_pieces[random_id]); // get<i>(tuple<...>) me devuelve el i-ésimo
        c_piece = get<0>(current_pieces[random_id]);  // elemento de la tupla
    }
    else
    {
        // Si no tengo fichas para el dado elegido, pasa turno (la macro SKIP_TURN me permite no mover).
        id_piece = SKIP_TURN;
        c_piece = actual->getCurrentColor(); // Le tengo que indicar mi color actual al pasar turno.
    }

}

void AIPlayer::thinkAleatorioMasInteligente(color &c_piece, int &id_piece, int &dice) const
{
// El número de mi jugador actual.
    int player = actual->getCurrentPlayerId();
// Vector que almacenará los dados que se pueden usar para el movimiento.
    vector<int> current_dices;
// Vector que almacenará los ids de las fichas que se pueden mover para el dado elegido.
    vector<tuple<color, int>> current_pieces;
// Obtengo el vector de dados que puedo usar para el movimiento.
// - actual->getAvailableNormalDices(player) -> solo dados disponibles en ese turno.
// Importante : "getAvailableNormalDices" me da los dados que puedo usar en el turno actual.
// Por ejemplo, si me tengo que contar 10 o 20 solo me saldrán los dados 10 y 20.
// Puedo saber qué más dados tengo, aunque no los pueda usar en este turno, con:
// - actual->getNormalDices(player) -> todos los dados
    current_dices = actual->getAvailableNormalDices(player);
// En vez de elegir un dado al azar, miro primero cuáles tienen fichas que se puedan mover.
    vector<int> current_dices_que_pueden_mover_ficha;

    for (int i = 0; i < current_dices.size(); i++)
    {
// Se obtiene el vector de fichas que se pueden mover para el dado elegido.
        current_pieces = actual->getAvailablePieces(player, current_dices[i]);
// Si se pueden mover fichas para el dado actual, lo añado al vector de dados que pueden mover fichas.
        if (current_pieces.size() > 0)
        {
            current_dices_que_pueden_mover_ficha.push_back(current_dices[i]);
        }
    }
// Si no tengo ningún dado que pueda mover fichas, paso turno con un dado al azar (la macro SKIP_TURN me permite no mover).
    if (current_dices_que_pueden_mover_ficha.size() == 0)
    {
        dice = current_dices[rand() % current_dices.size()];
        id_piece = SKIP_TURN;
        c_piece = actual->getCurrentColor(); // Le tengo que indicar mi color actual al pasar turno.
    }
// En caso contrario, elijo un dado de forma aleatoria de entre los que pueden mover ficha.
    else
    {
        dice = current_dices_que_pueden_mover_ficha[rand() % current_dices_que_pueden_mover_ficha.size()];
// Se obtiene el vector de fichas que se pueden mover para el dado elegido.
        current_pieces = actual->getAvailablePieces(player, dice);
// Muevo una ficha al azar de entre las que se pueden mover.
        int random_id = rand() % current_pieces.size();
        id_piece = get<1>(current_pieces[random_id]);
        c_piece = get<0>(current_pieces[random_id]);

    }
}

void AIPlayer::thinkFichaMasAdelantada(color &c_piece, int &id_piece, int &dice) const {
// Elijo el dado haciendo lo mismo que el jugador anterior.
    thinkAleatorioMasInteligente(c_piece, id_piece, dice);
// Tras llamar a esta función, ya tengo en dice el número de dado que quiero usar.
// Ahora, en vez de mover una ficha al azar, voy a mover (o a aplicar
// el dado especial a) la que esté más adelantada
// (equivalentemente, la más cercana a la meta).
    int player = actual->getCurrentPlayerId();
    vector <tuple<color, int>> current_pieces = actual->getAvailablePieces(player, dice);
    int id_ficha_mas_adelantada = -1;
    color col_ficha_mas_adelantada = none;
    int min_distancia_meta = 9999;
    for (int i = 0; i < current_pieces.size(); i++) {
// distanceToGoal(color, id) devuelve la distancia a la meta de la ficha [id] del color que le indique.
        color col = get<0>(current_pieces[i]);
        int id = get<1>(current_pieces[i]);
        int distancia_meta = actual->distanceToGoal(col, id);
        if (distancia_meta < min_distancia_meta) {
            min_distancia_meta = distancia_meta;
            id_ficha_mas_adelantada = id;
            col_ficha_mas_adelantada = col;
        }
    }
// Si no he encontrado ninguna ficha, paso turno.
    if (id_ficha_mas_adelantada == -1) {
        id_piece = SKIP_TURN;
        c_piece = actual->getCurrentColor(); // Le tengo que indicar mi color actual al pasar turno.
    }
// En caso contrario, moveré la ficha más adelantada.
    else {
        id_piece = id_ficha_mas_adelantada;
        c_piece = col_ficha_mas_adelantada;
    }
}

void AIPlayer::thinkMejorOpcion(color &c_piece, int &id_piece, int &dice) const
{
// Vamos a mirar todos los posibles movimientos del jugador actual accediendo a los hijos del estado actual.
// Cuando ya he recorrido todos los hijos, la función devuelve el estado actual. De esta forma puedo saber
// cuándo paro de iterar.
// Para ello, vamos a iterar sobre los hijos con la función de Parchis getChildren().
// Esta función devuelve un objeto de la clase ParchisBros, que es una estructura iterable
// sobre la que se pueden recorrer todos los hijos del estado sobre el que se llama.
    ParchisBros hijos = actual->getChildren();
    bool me_quedo_con_esta_accion = false;
// La clase ParchisBros viene con un iterador muy útil y sencillo de usar.
// Al hacer begin() accedemos al primer hijo de la rama,
// y cada vez que hagamos ++it saltaremos al siguiente hijo.
// Comparando con el iterador end() podemos consultar cuándo hemos terminado de visitar los hijos.
// Voy a moverme a la casilla siempre con la que gane más energía, salvo que me encuentre con
// algún movimiento muy interesante, como comer fichas o llegar a la meta.
    int current_power = actual->getPowerBar(this->jugador).getPower();
    int max_power = -101; // Máxima ganancia de energía.
    for (ParchisBros::Iterator it = hijos.begin();
         it != hijos.end() and !me_quedo_con_esta_accion; ++it)
    {
        Parchis siguiente_hijo = *it;
// Si en el movimiento elegido comiera ficha, llegara a la meta o ganara, me quedo con esa acción.
// Termino el bucle en este caso.
        if (siguiente_hijo.isEatingMove() or
            siguiente_hijo.isGoalMove() or
            (siguiente_hijo.gameOver() and siguiente_hijo.getWinner() == this->jugador))
        {
            me_quedo_con_esta_accion = true;
            c_piece = it.getMovedColor();
            id_piece = it.getMovedPieceId();
            dice = it.getMovedDiceValue();
        }
// En caso contrario, me voy quedando con el que me dé más energía.
        else{
            int new_power = siguiente_hijo.getPower(this->jugador);
            if(new_power - current_power > max_power){
                c_piece = it.getMovedColor();
                id_piece = it.getMovedPieceId();
                dice = it.getMovedDiceValue();
                max_power = new_power - current_power;
            }
        }
    }
// Si he encontrado una acción que me interesa, la guardo en las variables pasadas por referencia.
// (Ya lo he hecho antes, cuando les he asignado los valores con el iterador).
}

double AIPlayer::ValoracionTest(const Parchis &estado, int jugador)
{
    // Heurística de prueba proporcionada para validar el funcionamiento del algoritmo de búsqueda.


    int ganador = estado.getWinner();
    int oponente = (jugador+1) % 2;

    // Si hay un ganador, devuelvo más/menos infinito, según si he ganado yo o el oponente.
    if (ganador == jugador)
    {
        return gana;
    }
    else if (ganador == oponente)
    {
        return pierde;
    }
    else
    {
        // Colores que juega mi jugador y colores del oponente
        vector<color> my_colors = estado.getPlayerColors(jugador);
        vector<color> op_colors = estado.getPlayerColors(oponente);

        // Recorro todas las fichas de mi jugador
        int puntuacion_jugador = 0;
        // Recorro colores de mi jugador.
        for (int i = 0; i < my_colors.size(); i++)
        {
            color c = my_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++)
            {
                // Valoro positivamente que la ficha esté en casilla segura o meta.
                if (estado.isSafePiece(c, j))
                {
                    puntuacion_jugador++;
                }
                else if (estado.getBoard().getPiece(c, j).get_box().type == goal)
                {
                    puntuacion_jugador += 5;
                }
            }
        }

        // Recorro todas las fichas del oponente
        int puntuacion_oponente = 0;
        // Recorro colores del oponente.
        for (int i = 0; i < op_colors.size(); i++)
        {
            color c = op_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++)
            {
                if (estado.isSafePiece(c, j))
                {
                    // Valoro negativamente que la ficha esté en casilla segura o meta.
                    puntuacion_oponente++;
                }
                else if (estado.getBoard().getPiece(c, j).get_box().type == goal)
                {
                    puntuacion_oponente += 5;
                }
            }
        }

        // Devuelvo la puntuación de mi jugador menos la puntuación del oponente.
        return puntuacion_jugador - puntuacion_oponente;
    }
}

double AIPlayer::Heuristica(const Parchis &estado, int jugador) {
    // Heurística de prueba proporcionada para validar el funcionamiento del algoritmo de búsqueda.

    int ganador = estado.getWinner();
    int oponente = (jugador + 1) % 2;

    //para el dado
    int energia;

    // Si hay un ganador, devuelvo más/menos infinito, según si he ganado yo o el oponente.
    if (ganador == jugador) {
        return gana;
    } else if (ganador == oponente) {
        return pierde;
    } else {
        // Colores que juega mi jugador y colores del oponente
        vector<color> my_colors = estado.getPlayerColors(jugador);
        vector<color> op_colors = estado.getPlayerColors(oponente);

        // Recorro todas las fichas de mi jugador
        int puntuacion_jugador = 0;
        // Recorro colores de mi jugador.
        for (int i = 0; i < my_colors.size(); i++) {
            color c = my_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++) {
                // Valoro positivamente que la ficha esté en casilla segura o meta.
                if (estado.isSafePiece(c, j)) {
                    puntuacion_jugador++;
                } else if (estado.getBoard().getPiece(c, j).get_box().type == goal) {
                    puntuacion_jugador += 5;
                }
            }
        }

        // Recorro todas las fichas del oponente
        int puntuacion_oponente = 0;
        // Recorro colores del oponente.
        for (int i = 0; i < op_colors.size(); i++) {
            color c = op_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++) {
                if (estado.isSafePiece(c, j)) {
                    // Valoro negativamente que la ficha esté en casilla segura o meta.
                    puntuacion_oponente++;
                } else if (estado.getBoard().getPiece(c, j).get_box().type == goal) {
                    puntuacion_oponente += 5;
                }
            }
        }

        // Puntuación adicional por la ficha más cercana a la meta.
        for (int i = 0; i < my_colors.size(); i++) {
            color col = my_colors[i];
            for (int j = 0; j < num_pieces; j++) {
                puntuacion_jugador += 68 - estado.distanceToGoal(col, j);
            }
        }

//destruir fchas
        if (estado.isEatingMove()) { // si ha comido ficha
            auto piezaComida = estado.eatenPiece();
            puntuacion_jugador += (find(my_colors.begin(), my_colors.end(), piezaComida.first) != my_colors.end()) ? 1 : 80;
        } else if (estado.isGoalMove()) { // si ha metido ficha
            puntuacion_jugador += 40;
        } else { // si ha destruido pieza del rival suma, si destruye suya resta
            for (const auto& pieza : estado.piecesDestroyedLastMove()) {
                puntuacion_jugador += (find(my_colors.begin(), my_colors.end(), pieza.first) != my_colors.end()) ? -30 : 30;
            }
        }


        // Puntuación adicional por piezas en la meta.
        for (int i = 0; i < my_colors.size(); i++) {
            color co = my_colors[i];
            for (int j = 0; j < num_pieces; j++) {
                puntuacion_jugador += estado.piecesAtGoal(co) * 10;
            }
        }

        //Quitamos puntos al estar en casa
        for (int i = 0; i < my_colors.size(); i++) {
            color co = my_colors[i];
            puntuacion_oponente+=estado.piecesAtHome(co)*40;
        }




        //Para usar el dado
        //Damos puntos al usar el dado
        for (int i = 0; i < my_colors.size(); i++) {
            for (int j = 0; j < num_pieces; j++) {
                energia=estado.getPower(j);
                switch(energia){
                    case 0 ... 49://Movimiento rapido
                        puntuacion_jugador+=50;
                        break;
                    case 50 ... 59://Concha roja
                        puntuacion_jugador+=25;
                        break;
                    case 70 ... 74://Concha roja
                        puntuacion_jugador+=25;
                        break;
                    case 60 ... 64://BOOM
                        puntuacion_jugador+=10;
                        break;
                    case 65 ... 69://Movimiento ultra rapido
                        puntuacion_jugador+=35;
                        break;
                    case 75 ... 79://Movimietno bala
                        puntuacion_jugador+=40;
                        break;
                    case 80 ... 84://CATAPUN
                        puntuacion_jugador+=35;
                        break;
                    case 85 ... 89://Concha azul
                        puntuacion_jugador+=30;
                        break;
                    case 90 ... 94://BOOMBOOM
                        puntuacion_jugador+=15;
                        break;
                    case 95 ... 99://Mvimietno estreña
                        puntuacion_jugador+=35;
                        break;
                    case 100://CATAPUMCHIMPUM
                        puntuacion_jugador+=25;
                        break;

                }
            }
        }


        // Devuelvo la puntuación de mi jugador menos la puntuación del oponente.
        return puntuacion_jugador - puntuacion_oponente;
    }
}


bool AIPlayer::move(){
    cout << "Realizo un movimiento automatico" << endl;

    color c_piece;
    int id_piece;
    int dice;
    think(c_piece, id_piece, dice);

    cout << "Movimiento elegido: " << str(c_piece) << " " << id_piece << " " << dice << endl;

    actual->movePiece(c_piece, id_piece, dice);
    return true;
}

double AIPlayer::Poda_AlfaBeta(const Parchis &actual, int jugador, int profundidad,int profundidad_max, color &c_piece,
                               int &id_piece, int &dice, double alpha, double beta, double (*heuristic)(const Parchis &, int)) const {
    bool mejor = false;
    double k;
    ParchisBros hijo = actual.getChildren();


    if (profundidad == profundidad_max || actual.gameOver()) {
        return heuristic(actual, jugador);
    }


    if (actual.getCurrentPlayerId() == jugador) {
        mejor = true;
    }


    if (mejor) {
        for (auto it = hijo.begin(); it != hijo.end(); ++it) {
            k = Poda_AlfaBeta(*it, jugador, profundidad + 1, profundidad_max,
                              c_piece, id_piece, dice, alpha, beta, heuristic);

            if (k > alpha) {
                alpha = k;
                if (profundidad == 0) {
                    c_piece = it.getMovedColor();
                    id_piece = it.getMovedPieceId();
                    dice = it.getMovedDiceValue();
                }
            }


            if (alpha >= beta) {
                return alpha;
            }
        }
        return alpha;
    } else {
        for (auto it = hijo.begin(); it != hijo.end(); ++it) {
            k = Poda_AlfaBeta(*it, jugador, profundidad + 1,
                              profundidad_max, c_piece, id_piece, dice, alpha, beta, heuristic);

            if (k < beta) {
                beta = k;
            }


            if (beta <= alpha) {
                return beta;
            }
        }
        return beta;
    }
}



void AIPlayer::think(color & c_piece, int & id_piece, int & dice) const{



    // El siguiente código se proporciona como sugerencia para iniciar la implementación del agente.

    double valor; // Almacena el valor con el que se etiqueta el estado tras el proceso de busqueda.
    double alpha = menosinf, beta = masinf; // Cotas iniciales de la poda AlfaBeta
    // Llamada a la función para la poda (los parámetros son solo una sugerencia, se pueden modificar).
    valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, ValoracionTest);
    cout << "Valor MiniMax: " << valor << "  Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;

    // ----------------------------------------------------------------- //

    // Si quiero poder manejar varias heurísticas, puedo usar la variable id del agente para usar una u otra.
    switch(id){
        case 0:
            valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, ValoracionTest);
            break;
        case 1:
            valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, Heuristica);
            break;
        case 2:
           valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, Heuristica);
            break;
    }
    cout << "Valor MiniMax: " << valor << "  Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;

/*
    switch(id){
        case 0:
            thinkAleatorio(c_piece, id_piece, dice);
            break;
        case 1:
            thinkAleatorioMasInteligente(c_piece, id_piece, dice);
            break;
        case 2:
            thinkFichaMasAdelantada(c_piece, id_piece, dice);
            break;
        case 3:
            thinkMejorOpcion(c_piece, id_piece, dice);
            break;
    }
*/
}

