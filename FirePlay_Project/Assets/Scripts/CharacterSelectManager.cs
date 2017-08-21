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

	private Sprite _pointer = null;
	private GameObject _archer1 = null;
	private GameObject _archer2 = null;
	private GameObject _archer3 = null;

	private void Start()
	{
		SetArchers();
	}

	private void SetArchers()
	{
		_archer1 = Instantiate(Resources.Load("PrivateData/SpritesArchers/FantasyArcher_01") as GameObject);
		_archer1.GetComponent<Transform>().position = new Vector3(-4f, 0f, 0f);

		_archer2 = Instantiate(Resources.Load("PrivateData/SpritesArchers/FantasyArcher_02") as GameObject);
		_archer2.GetComponent<Transform>().position = new Vector3(0f, 0f, 0f);

		_archer3 = Instantiate(Resources.Load("PrivateData/SpritesArchers/FantasyArcher_03") as GameObject);
		_archer3.GetComponent<Transform>().position = new Vector3(4f, 0f, 0f);
	}
}
