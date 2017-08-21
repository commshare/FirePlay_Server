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
	private GameObject _archer1 = null;
	private GameObject _archer2 = null;
	private GameObject _archer3 = null;

	private void Start()
	{
		LoadArchers();
	}

	private void LoadArchers()
	{
		_archer1 = Instantiate(Resources.Load("PrivateData/SpritesArchers/FantasyArcher_01") as GameObject);
		_archer2 = Instantiate(Resources.Load("PrivateData/SpritesArchers/FantasyArcher_02") as GameObject);
		_archer3 = Instantiate(Resources.Load("PrivateData/SpritesArchers/FantasyArcher_01") as GameObject);
	}
}
