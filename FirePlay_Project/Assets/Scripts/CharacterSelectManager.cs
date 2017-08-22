using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public enum PlayerType : int
{
	None = 0,
	Archer1 = 1,
	Archer2 = 2,
	Archer3 = 3
}

public class CharacterSelectManager : MonoBehaviour
{
	public PlayerType _selectedCharacter = PlayerType.Archer1;

	private GameObject _pointer = null;
	private GameObject _archer1 = null;
	private GameObject _archer2 = null;
	private GameObject _archer3 = null;
    private Vector3[] _archerPosition = new Vector3[(int)PlayerType.Archer3];

	private void Start()
	{
		SetArchers();
		SetPointer();
	}

	private void Update()
	{
		GetPointerMove();		
	}

	private void SetArchers()
	{
		_archer1 = Instantiate(Resources.Load("PrivateData/SpritesArchers/FantasyArcher_01") as GameObject);
        _archerPosition[(int)PlayerType.Archer1] = new Vector3(-4f, 0f, 0f);
        _archer1.GetComponent<Transform>().position = _archerPosition[(int)PlayerType.Archer1];

		_archer2 = Instantiate(Resources.Load("PrivateData/SpritesArchers/FantasyArcher_02") as GameObject);
        _archerPosition[(int)PlayerType.Archer2] = new Vector3(0f, 0f, 0f);
        _archer2.GetComponent<Transform>().position = _archerPosition[(int)PlayerType.Archer2];

		_archer3 = Instantiate(Resources.Load("PrivateData/SpritesArchers/FantasyArcher_03") as GameObject);
        _archerPosition[(int)PlayerType.Archer2] = new Vector3(4f, 0f, 0f);
        _archer2.GetComponent<Transform>().position = _archerPosition[(int)PlayerType.Archer2];
	}

	private void SetPointer()
	{
		_pointer = Instantiate(Resources.Load("PrivateData/SelectPointer") as GameObject);
		_pointer.GetComponent<Transform>().position = new Vector3(-4f, -3f, 0f);
	}

	private void GetPointerMove()
	{
		if (Input.GetKey(KeyCode.LeftArrow))
		{

		}
		else if (Input.GetKey(KeyCode.RightArrow))
		{

		}
	}
}
