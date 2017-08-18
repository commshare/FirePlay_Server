using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class LoginManager : MonoBehaviour
{
	public InputField _idField;
	public InputField _pwField;
	public string _id;
	public string _pw;

	private void OnGUI()
	{
		ButtonCheck();
	}

	private void Start()
	{
		SetBackground();	
		MakeInputFields();
	}

	private void SetBackground()
	{
		var prefab = Resources.Load("Prefabs/Background") as GameObject;
		var backgroundInstance = Instantiate(prefab).GetComponent<Background>();

		backgroundInstance.Init();
	}

	private void ButtonCheck()
	{
		if (GUI.Button(new Rect((Screen.width / 2) - 155, Screen.height * 2 / 3, 150, 50), "Login"))
		{

		}

		if (GUI.Button(new Rect((Screen.width / 2) + 5, Screen.height * 2 / 3, 150, 50), "Exit"))
		{

		}
	}

	private void MakeInputFields()
	{
		// 초기값 세팅.
		_idField.text = "ID";
		_pwField.text = "Password";

		var idEvent = new InputField.SubmitEvent();
		idEvent.AddListener(GetId);
		_idField.onEndEdit = idEvent;

		var pwEvent = new InputField.SubmitEvent();
		pwEvent.AddListener(GetPw);
		_pwField.onEndEdit = pwEvent;
	}

	private void GetId(string arg0)
	{
		_id = arg0;
	}

	private void GetPw(string arg0)
	{
		_pw = arg0;
	}
}
