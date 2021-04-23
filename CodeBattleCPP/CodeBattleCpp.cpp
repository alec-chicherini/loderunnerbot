#include <iostream>
#include <random>

#include "details/BeastGameClient.h"
#include "GameBoard.h"
#include "Graph.h"

#define DEBUG_SEQUENCE
//#define DEBUG_PAUSE

bool isBaseGraphMade = false;
static Graph Gr;

bool isCurrentRouteInWork = false;
bool recreate = false;
bool isMoveComplete = true;

BoardPoint nextStep;


std::vector<LodeRunnerAction> LEFT_DRILLRIGHT_RIGTH_SEQUENCE
{
	LodeRunnerAction::GO_LEFT,
	LodeRunnerAction::DRILL_RIGHT,
	LodeRunnerAction::GO_RIGHT,
	LodeRunnerAction::GO_RIGHT
};

std::vector<LodeRunnerAction> RIGHT_DRILLLEFT_LEFT_SEQUENCE
{
	LodeRunnerAction::GO_RIGHT,
	LodeRunnerAction::DRILL_LEFT,
	LodeRunnerAction::GO_LEFT,
	LodeRunnerAction::GO_LEFT
};

enum class SEQUENCE
{
	NONE,
	SKIP_1_TICK,
	LEFT_DRILLRIGHT_RIGHT,
	RIGHT_DRILLLEFT_LEFT
};

SEQUENCE current_sequence = SEQUENCE::NONE;
int sequence_step=0;

bool isGamePaused=true;
BoardPoint prev_enemy_state;

bool is_this_first_tick = true;

LodeRunnerAction makeTurn(const GameBoard& board) 
{
#ifdef DEBUG_PAUSE
 std::cout << " was- [" << prev_enemy_state.getX() << "," << prev_enemy_state.getY() << "]"
		   << " now- [" << board.getEnemyPositions().back().getX() << "," << board.getEnemyPositions().back().getY() << "]"
		   <<std::endl;
#endif // DEBUG_PAUSE

 auto current_enemy_state= board.getEnemyPositions().back();

 if (is_this_first_tick)
 {

	 prev_enemy_state = current_enemy_state;
	 is_this_first_tick = false;
	 if (!recreate) { recreate = true; return LodeRunnerAction::SUICIDE; }
 };

 if ((prev_enemy_state.getX() == current_enemy_state.getX()) &&
	 (prev_enemy_state.getY() == current_enemy_state.getY()))isGamePaused = true;
 else isGamePaused = false;

 prev_enemy_state = current_enemy_state;

	if (isGamePaused)
	{ 

		std::cout << "GAME is PAUSED"<<std::endl;
		current_sequence = SEQUENCE::NONE;
		sequence_step = 0;

		isBaseGraphMade = false;
		Gr.cleanUp();

		isCurrentRouteInWork = false;
		recreate = false;
		isMoveComplete = true;

		return LodeRunnerAction::GO_UP;
	}



	if (current_sequence == SEQUENCE::NONE)
	{


#ifdef DEBUG_SEQUENCE
		std::cout << "current_sequence = " << static_cast<int>(current_sequence);
#endif

		
		const BoardPoint& position = board.getMyPosition();
		std::cout << " Hero position is [" << position.getX() << "," << position.getY() << "]\n";

		if (!isBaseGraphMade)
			isBaseGraphMade = Gr.BuildGraphFromMap(board);

		if (!isCurrentRouteInWork) {
#ifdef DEBUG_SEQUENCE
			std::cout << "createRouteWithBFS begin"<<std::endl;
#endif
			Gr.createRouteWithBFS(position, board);
			Gr.printCurrentRoute();
			isCurrentRouteInWork = true;
#ifdef DEBUG_SEQUENCE
			std::cout << "createRouteWithBFS end" << std::endl;
#endif
		}

		if (isMoveComplete||(position==nextStep))
		{
#ifdef DEBUG_SEQUENCE
			std::cout << "isMoveComplete begin" << std::endl;
#endif
			nextStep = Gr.makeStep();
			isMoveComplete = false;
			if (Gr.isCurrentRouteEmpty())
			{
				isCurrentRouteInWork = false;
			}
#ifdef DEBUG_SEQUENCE
			std::cout << "isMoveComplete end" << std::endl;
#endif
		};

		//left
		if (nextStep.getX() < position.getX())
		{
#ifdef DEBUG_SEQUENCE
			std::cout << "LEFT: (nextStep.getX()= "<< nextStep.getX()<<" < " 
				      << "position.getX() = " << position.getX() << " ) "<< std::endl;
#endif
			isMoveComplete = true;
			return LodeRunnerAction::GO_LEFT;
		}
		//rigth
		else if (nextStep.getX() > position.getX())
		{
#ifdef DEBUG_SEQUENCE
			std::cout << "RIGHT: (nextStep.getX()= " << nextStep.getX() << " > "
					  << "position.getX() = " << position.getX() << " ) " << std::endl;
#endif
			isMoveComplete = true;
			return LodeRunnerAction::GO_RIGHT;
		}
		//down
		else if (nextStep.getY() > position.getY())
		{
#ifdef DEBUG_SEQUENCE
			std::cout << "DOWN: (nextStep.getY()= " << nextStep.getY() << " > "
					  << "position.getY() = " << position.getY() << " ) " << std::endl;
#endif
			if (isLadder(board.getElementAt(nextStep)) ||
				isLikeNONE(board.getElementAt(nextStep))||
				isPipe(board.getElementAt(nextStep))||
				isNONE(board.getElementAt(nextStep)))
				
			{
				isMoveComplete = true;
				return LodeRunnerAction::GO_DOWN;
			}

			if (board.hasElementAt(nextStep, BoardElement::BRICK))
			{
				
				if (
					Gr.isReachable(BoardPoint(position.getX() - 1, position.getY())) &&
					isWalkable(board.getElementAt(BoardPoint(position.getX() - 1, position.getY() + 1)))
					)
				{
#ifdef DEBUG_SEQUENCE
					std::cout << "SEQUENCE::LEFT_DRILLRIGHT_RIGHT started " << std::endl;
#endif
					current_sequence = SEQUENCE::LEFT_DRILLRIGHT_RIGHT;
					sequence_step = 0;
					return LEFT_DRILLRIGHT_RIGTH_SEQUENCE[sequence_step++];
				};

				if (
					Gr.isReachable(BoardPoint(position.getX() + 1, position.getY())) &&
					isWalkable(board.getElementAt(BoardPoint(position.getX() + 1, position.getY() + 1)))
					)
				{
#ifdef DEBUG_SEQUENCE
					std::cout << "SEQUENCE::RIGHT_DRILLLEFT_LEFT started " << std::endl;
#endif
					current_sequence = SEQUENCE::RIGHT_DRILLLEFT_LEFT;
					sequence_step = 0;
					return RIGHT_DRILLLEFT_LEFT_SEQUENCE[sequence_step++];

				};

			};
		}
		//up
		else if (nextStep.getY() < position.getY())
		{
#ifdef DEBUG_SEQUENCE
			std::cout << "UP: (nextStep.getY() = " << nextStep.getY() << " < "
						   << "position.getY() = " << position.getY() << " ) " << std::endl;
#endif
			isMoveComplete = true;
			return LodeRunnerAction::GO_UP;
		}
	}

	else
		switch (current_sequence)
		{
		case SEQUENCE::LEFT_DRILLRIGHT_RIGHT:
		{
#ifdef DEBUG_SEQUENCE
			std::cout << "case SEQUENCE::LEFT_DRILLRIGHT_RIGHT: " << std::endl;
			std::cout << "current_sequence = " << static_cast<int>(current_sequence) << std::endl;
			std::cout << "sequence_step = " << sequence_step << std::endl;
#endif
			if (sequence_step != LEFT_DRILLRIGHT_RIGTH_SEQUENCE.size() - 1)
				return LEFT_DRILLRIGHT_RIGTH_SEQUENCE[sequence_step++];
			else current_sequence = SEQUENCE::SKIP_1_TICK;

			break;
		}
		case SEQUENCE::RIGHT_DRILLLEFT_LEFT:
		{
#ifdef DEBUG_SEQUENCE
			std::cout << "case SEQUENCE::RIGHT_DRILLLEFT_LEFT: " << std::endl;
			std::cout << "current_sequence = " << static_cast<int>(current_sequence) << std::endl;
			std::cout << "sequence_step = " << sequence_step << std::endl;
#endif
			if (sequence_step != RIGHT_DRILLLEFT_LEFT_SEQUENCE.size() - 1)
				return RIGHT_DRILLLEFT_LEFT_SEQUENCE[sequence_step++];
			else current_sequence = SEQUENCE::SKIP_1_TICK;

			break;
		}
		case SEQUENCE::SKIP_1_TICK:
		{
#ifdef DEBUG_SEQUENCE
			std::cout << "case SEQUENCE::SKIP_1_TICK: " << std::endl;
			std::cout << "current_sequence = " << static_cast<int>(current_sequence) << std::endl;
			std::cout << "sequence_step = " << sequence_step << std::endl;
#endif
			sequence_step = 0;
			current_sequence = SEQUENCE::NONE;
			isMoveComplete = true;
			break;
		}
		default:
			break;
		}


};

int main() {
	const std::string serverUrl = "http://dojorena.io/codenjoy-contest/board/player/dojorena329?code=6433848582930553241";

	while(1) {
		details::BeastGameClient gcb(serverUrl);
		gcb.Run(makeTurn);
	}

	return 0;
}
