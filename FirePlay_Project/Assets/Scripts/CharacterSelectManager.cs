using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public enum PlayerType
{
	None = 0,
	Archer1 = 1,
	Archer2 = 2,
	Archer3 = 3
}

public class CharacterSelectManager : MonoBehaviour
{
	public PlayerType _selectedCharacter = PlayerType.Archer1;
	public Sprite _pointer = null;
	public Animator _archer1 = null; 

	private void Start()
	{
		LoadArchers();
	}

	private void LoadArchers()
	{
	}
}
