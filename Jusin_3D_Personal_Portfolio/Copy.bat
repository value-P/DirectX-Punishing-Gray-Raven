// ��ɾ�				�ɼ�				���� ������ �ִ� ��ġ			�纻 ������ ������ ��ġ

xcopy				/y/s			.\Engine\Public\*.*						.\Reference\Headers\

xcopy				/y				.\Engine\Bin\Engine.dll					.\Client\Bin\
xcopy				/y				.\Engine\Bin\Engine.dll					.\Tool\Bin\
xcopy				/y				.\Engine\Bin\Engine.lib					.\Reference\Librarys\
xcopy				/y				.\Engine\ThirdPartyLib\*.lib			.\Reference\Librarys\

xcopy				/y				.\Engine\Bin\ShaderFiles\*.*			.\Client\Bin\ShaderFiles\
xcopy				/y				.\Engine\Bin\ShaderFiles\*.*			.\Tool\Bin\ShaderFiles\

xcopy				/y				.\Tool\Bin\AnimEventData\*.*			.\Client\Bin\AnimEventData\
xcopy				/y				.\Tool\Bin\EffectEventData\*.*			.\Client\Bin\EffectEventData\
xcopy				/y				.\Tool\Bin\EffectPrefab\*.*				.\Client\Bin\EffectPrefab\