using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class LoginManager : MonoBehaviour
{
	private void OnGUI()
	{
		if (GUI.Button(new Rect(10, 10, 150, 100), "Login"))
		{
			
		}

		if (GUI.Button(new Rect(170, 10, 150, 100), "Exit"))
		{

		}
	}

	private void Start()
	{
		SetBackground();	
	}

	private void SetBackground()
	{
		var prefab = Resources.Load("Prefabs/Background") as GameObject;
		var instance = Instantiate(prefab).GetComponent<Background>();

		instance.Init();
	}
}
