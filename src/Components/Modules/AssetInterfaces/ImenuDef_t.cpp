#include "STDInclude.hpp"

namespace Assets
{
	void ImenuDef_t::mark(Game::XAssetHeader header, Components::ZoneBuilder::Zone* builder)
	{
		Game::menuDef_t *asset = header.menu;

		if (asset->window.background)
		{
			builder->loadAsset(Game::XAssetType::ASSET_TYPE_MATERIAL, asset->window.background);
		}
		
		// mark items
		for (int i = 0; i < asset->itemCount; ++i)
		{
			if (asset->items[i]->window.background)
			{
				builder->loadAsset(Game::XAssetType::ASSET_TYPE_MATERIAL, asset->items[i]->window.background);
			}

			if (asset->items[i]->focusSound)
			{
				builder->loadAsset(Game::XAssetType::ASSET_TYPE_SOUND, asset->items[i]->focusSound);
			}

			if (asset->items[i]->type == 6 && asset->items[i]->typeData.listBox &&
				asset->items[i]->typeData.listBox->selectIcon)
			{
				builder->loadAsset(Game::XAssetType::ASSET_TYPE_MATERIAL, asset->items[i]->typeData.listBox->selectIcon);
			}
		}
	}

	void ImenuDef_t::save_ExpressionSupportingData(Game::ExpressionSupportingData* asset, Components::ZoneBuilder::Zone* builder)
	{
		AssertSize(Game::ExpressionSupportingData, 24);
		Utils::Stream* buffer = builder->getBuffer();

		buffer->align(Utils::Stream::ALIGN_4);

		Game::ExpressionSupportingData *dest = buffer->dest<Game::ExpressionSupportingData>();
		buffer->save(asset);

		if (asset->uifunctions.functions)
		{
			buffer->align(Utils::Stream::ALIGN_4);

			Game::Statement_s **destStatement = buffer->dest<Game::Statement_s*>();
			buffer->saveArray(asset->uifunctions.functions, asset->uifunctions.totalFunctions);

			for (int i = 0; i < asset->uifunctions.totalFunctions; ++i)
			{
				if (asset->uifunctions.functions[i])
				{
					Utils::Stream::ClearPointer(&destStatement[i]);

					buffer->align(Utils::Stream::ALIGN_4);
					this->save_Statement_s(asset->uifunctions.functions[i], builder);
				}
			}

			Utils::Stream::ClearPointer(&dest->uifunctions.functions);
		}

		if (asset->staticDvarList.staticDvars)
		{
			buffer->align(Utils::Stream::ALIGN_4);

			Game::StaticDvar **destStaticDvars = buffer->dest<Game::StaticDvar*>();
			buffer->saveArray(asset->staticDvarList.staticDvars, asset->staticDvarList.numStaticDvars);

			for (int i = 0; i < asset->staticDvarList.numStaticDvars; ++i)
			{
				if (asset->staticDvarList.staticDvars[i])
				{
					Utils::Stream::ClearPointer(&destStaticDvars[i]);

					buffer->align(Utils::Stream::ALIGN_4);
					Game::StaticDvar *destStaticDvar = buffer->dest<Game::StaticDvar>();
					buffer->save(asset->staticDvarList.staticDvars[i]);

					if (asset->staticDvarList.staticDvars[i]->dvarName)
					{
						buffer->saveString(asset->staticDvarList.staticDvars[i]->dvarName);
						Utils::Stream::ClearPointer(&destStaticDvar->dvarName);
					}
				}
			}

			Utils::Stream::ClearPointer(&dest->staticDvarList.staticDvars);
		}

		if (asset->uiStrings.strings)
		{
			buffer->align(Utils::Stream::ALIGN_4);

			const char **destuiStrings = buffer->dest<const char*>();
			buffer->saveArray(asset->uiStrings.strings, asset->uiStrings.totalStrings);

			for (int i = 0; i < asset->uiStrings.totalStrings; ++i)
			{
				if (asset->uiStrings.strings[i])
				{
					buffer->saveString(asset->uiStrings.strings[i]);
					Utils::Stream::ClearPointer(&destuiStrings[i]);
				}
			}
		}
	}

	void ImenuDef_t::save_Statement_s(Game::Statement_s* asset, Components::ZoneBuilder::Zone* builder)
	{
		AssertSize(Game::Statement_s, 24);
		Utils::Stream* buffer = builder->getBuffer();

		// Write header data
		Game::Statement_s *dest = buffer->dest<Game::Statement_s>();
		buffer->save(asset);

		// Write statement entries
		if (asset->entries)
		{
			buffer->align(Utils::Stream::ALIGN_4);

			// Write entries
			Game::expressionEntry *destEntries = buffer->dest<Game::expressionEntry>();
			buffer->save(asset->entries, sizeof(Game::expressionEntry), asset->numEntries);

			// Loop through entries
			for (int i = 0; i < asset->numEntries; ++i)
			{
				if (asset->entries[i].type)
				{
					switch (asset->entries[i].data.operand.dataType)
					{
						// Those types do not require additional data
					case 0:
					case 1:
						break;

						// Expression string
					case 2:
						if (asset->entries[i].data.operand.internals.stringVal.string)
						{
							buffer->saveString(asset->entries[i].data.operand.internals.stringVal.string);
							Utils::Stream::ClearPointer(&destEntries[i].data.operand.internals.stringVal.string);
						}
						break;

						// Function
					case 3:
						if (asset->entries[i].data.operand.internals.function)
						{
							buffer->align(Utils::Stream::ALIGN_4);
							this->save_Statement_s(asset->entries[i].data.operand.internals.function, builder);
							Utils::Stream::ClearPointer(&destEntries[i].data.operand.internals.function);
						}
						break;
					}
				}
			}
		}
		if (asset->supportingData)
		{
			this->save_ExpressionSupportingData(asset->supportingData, builder);
			Utils::Stream::ClearPointer(&dest->supportingData);
		}
	}

	void ImenuDef_t::save_MenuEventHandlerSet(Game::MenuEventHandlerSet* asset, Components::ZoneBuilder::Zone* builder)
	{
		AssertSize(Game::MenuEventHandlerSet, 8);
		Utils::Stream* buffer = builder->getBuffer();

		// Write header data
		Game::MenuEventHandlerSet *destset = buffer->dest<Game::MenuEventHandlerSet>();
		buffer->save(asset);

		// Event handlers
		if (asset->eventHandlers)
		{
			buffer->align(Utils::Stream::ALIGN_4);

			// Write pointers to zone
			buffer->save(asset->eventHandlers, sizeof(Game::MenuEventHandler*), asset->eventHandlerCount);

			// Loop through eventHandlers
			for (int i = 0; i < asset->eventHandlerCount; ++i)
			{
				if (asset->eventHandlers[i])
				{
					buffer->align(Utils::Stream::ALIGN_4);

					// Write menu event handler
					Game::MenuEventHandler *dest = buffer->dest<Game::MenuEventHandler>();
					buffer->save(asset->eventHandlers[i]);

					// Write additional data based on type
					switch (asset->eventHandlers[i]->eventType)
					{
						// unconditional scripts
					case 0:
						if (asset->eventHandlers[i]->eventData.unconditionalScript)
						{
							buffer->saveString(asset->eventHandlers[i]->eventData.unconditionalScript);
							Utils::Stream::ClearPointer(&dest->eventData.unconditionalScript);
						}
						break;

						// ConditionalScript
					case 1:
						if (asset->eventHandlers[i]->eventData.conditionalScript)
						{
							buffer->align(Utils::Stream::ALIGN_4);
							Game::ConditionalScript *destConditionalScript = buffer->dest<Game::ConditionalScript>();
							buffer->save(asset->eventHandlers[i]->eventData.conditionalScript);

							// eventExpression
							if (asset->eventHandlers[i]->eventData.conditionalScript->eventExpression)
							{
								buffer->align(Utils::Stream::ALIGN_4);
								this->save_Statement_s(asset->eventHandlers[i]->eventData.conditionalScript->eventExpression, builder);
								Utils::Stream::ClearPointer(&destConditionalScript->eventExpression);
							}

							// eventHandlerSet
							if (asset->eventHandlers[i]->eventData.conditionalScript->eventHandlerSet)
							{
								buffer->align(Utils::Stream::ALIGN_4);
								this->save_MenuEventHandlerSet(asset->eventHandlers[i]->eventData.conditionalScript->eventHandlerSet, builder);
								Utils::Stream::ClearPointer(&destConditionalScript->eventHandlerSet);
							}

							Utils::Stream::ClearPointer(&dest->eventData.conditionalScript);
						}
						break;

						// elseScript
					case 2:
						if (asset->eventHandlers[i]->eventData.elseScript)
						{
							buffer->align(Utils::Stream::ALIGN_4);
							this->save_MenuEventHandlerSet(asset->eventHandlers[i]->eventData.elseScript, builder);
							Utils::Stream::ClearPointer(&dest->eventData.elseScript);
						}
						break;

						// localVarData expressions
					case 3:
					case 4:
					case 5:
					case 6:
						if (asset->eventHandlers[i]->eventData.setLocalVarData)
						{
							buffer->align(Utils::Stream::ALIGN_4);

							// header data
							Game::SetLocalVarData *destLocalVarData = buffer->dest<Game::SetLocalVarData>();
							buffer->save(asset->eventHandlers[i]->eventData.setLocalVarData);

							// localVarName
							if (asset->eventHandlers[i]->eventData.setLocalVarData->localVarName)
							{
								buffer->saveString(asset->eventHandlers[i]->eventData.setLocalVarData->localVarName);
								Utils::Stream::ClearPointer(&destLocalVarData->localVarName);
							}

							// statement
							if (asset->eventHandlers[i]->eventData.setLocalVarData->expression)
							{
								buffer->align(Utils::Stream::ALIGN_4);
								this->save_Statement_s(asset->eventHandlers[i]->eventData.setLocalVarData->expression, builder);
								Utils::Stream::ClearPointer(&destLocalVarData->expression);
							}

							Utils::Stream::ClearPointer(&dest->eventData.setLocalVarData);
						}
						break;
					}
				}
			}

			Utils::Stream::ClearPointer(&destset->eventHandlers);
		}
	}

	void ImenuDef_t::save_ItemKeyHandler(Game::ItemKeyHandler* asset, Components::ZoneBuilder::Zone* builder)
	{
		AssertSize(Game::ItemKeyHandler, 12);
		Utils::Stream* buffer = builder->getBuffer();

		while (asset)
		{
			// Write header
			Game::ItemKeyHandler* dest = buffer->dest<Game::ItemKeyHandler>();
			buffer->save(asset);

			// MenuEventHandlerSet
			if (asset->action)
			{
				buffer->align(Utils::Stream::ALIGN_4);
				this->save_MenuEventHandlerSet(asset->action, builder);
				Utils::Stream::ClearPointer(&dest->action);
			}

			if (asset->next)
			{
				// align every indice, besides the first one?
				buffer->align(Utils::Stream::ALIGN_4);
			}

			// Next key handler
			asset = asset->next;
		}
	}

#define EVENTHANDLERSET(__indice) \
		if (asset->__indice) \
		{ \
			buffer->align(Utils::Stream::ALIGN_4); \
			this->save_MenuEventHandlerSet(asset->__indice, builder); \
			Utils::Stream::ClearPointer(&dest->__indice); \
		}

#define STATEMENT(__indice) \
		if (asset->__indice) \
		{ \
			buffer->align(Utils::Stream::ALIGN_4); \
			this->save_Statement_s(asset->__indice, builder); \
			Utils::Stream::ClearPointer(&dest->__indice); \
		}

	void ImenuDef_t::save_itemDefData_t(Game::itemDefData_t* asset, int type, Game::itemDef_t* dest, Components::ZoneBuilder::Zone* builder)
	{
		AssertSize(Game::newsTickerDef_s, 28);
		AssertSize(Game::listBoxDef_s, 324);
		AssertSize(Game::editFieldDef_s, 32);
		AssertSize(Game::multiDef_s, 392);

		Utils::Stream* buffer = builder->getBuffer();

		// feeder
		if (type == 6)
		{
			buffer->align(Utils::Stream::ALIGN_4);
			Game::listBoxDef_s* destlb = buffer->dest<Game::listBoxDef_s>();
			buffer->save(asset->listBox);

			if (asset->listBox->doubleClick)
			{
				buffer->align(Utils::Stream::ALIGN_4);
				this->save_MenuEventHandlerSet(asset->listBox->doubleClick, builder);
			}

			if (asset->listBox->selectIcon)
			{
				destlb->selectIcon = builder->saveSubAsset(Game::XAssetType::ASSET_TYPE_MATERIAL, asset->listBox->selectIcon).material;
			}
		}
		// HexRays spaghetti
		else if (type != 4
			&& type != 9
			&& type != 16
			&& type != 18
			&& type != 11
			&& type != 14
			&& type != 10
			&& type != 17
			&& type != 22
			&& type != 23
			&& type != 0)
		{
			switch (type)
			{
				// enum dvar
			case 13:
				buffer->saveString(asset->enumDvarName);
				break;
				// newsticker
			case 20:
				buffer->align(Utils::Stream::ALIGN_4);
				buffer->save(asset->ticker);
				break;
				// textScrollDef
			case 21:
				buffer->align(Utils::Stream::ALIGN_4);
				buffer->save(asset->scroll);
				break;
			case 12:
				buffer->align(Utils::Stream::ALIGN_4);
				Game::multiDef_s* destdef = buffer->dest<Game::multiDef_s>();
				buffer->save(asset->multiDef);

				for (int i = 0; i < 32; ++i)
				{
					if (asset->multiDef->dvarList[i])
					{
						buffer->saveString(asset->multiDef->dvarList[i]);
						Utils::Stream::ClearPointer(&destdef->dvarList[i]);
					}
				}

				for (int i = 0; i < 32; ++i)
				{
					if (asset->multiDef->dvarStr[i])
					{
						buffer->saveString(asset->multiDef->dvarStr[i]);
						Utils::Stream::ClearPointer(&destdef->dvarStr[i]);
					}
				}

				break;
			}
		}
		// editFieldDef
		else
		{
			buffer->align(Utils::Stream::ALIGN_4);
			buffer->save(asset->editField);
		}

		Utils::Stream::ClearPointer(&dest->typeData.data);
	}

	void ImenuDef_t::save_itemDef_t(Game::itemDef_t *asset, Components::ZoneBuilder::Zone* builder)
	{
		AssertSize(Game::itemDef_t, 380);

		Utils::Stream* buffer = builder->getBuffer();
		Game::itemDef_t* dest = buffer->dest<Game::itemDef_t>();

		buffer->save(asset);

		// window data
		save_windowDef_t<Game::itemDef_t>(&asset->window, dest, builder);

		// text
		if (asset->text)
		{
			buffer->saveString(asset->text);
			Utils::Stream::ClearPointer(&dest->text);
		}

		// MenuEventHandlerSets
		EVENTHANDLERSET(mouseEnterText);
		EVENTHANDLERSET(mouseExitText);
		EVENTHANDLERSET(mouseEnter);
		EVENTHANDLERSET(mouseExit);
		EVENTHANDLERSET(action);
		EVENTHANDLERSET(accept);
		EVENTHANDLERSET(onFocus);
		EVENTHANDLERSET(leaveFocus);

		// Dvar strings
		if (asset->dvar)
		{
			buffer->saveString(asset->dvar);
			Utils::Stream::ClearPointer(&dest->dvar);
		}
		if (asset->dvarTest)
		{
			buffer->saveString(asset->dvarTest);
			Utils::Stream::ClearPointer(&dest->dvarTest);
		}

		// ItemKeyHandler
		if (asset->onKey)
		{
			buffer->align(Utils::Stream::ALIGN_4);
			this->save_ItemKeyHandler(asset->onKey, builder);
			Utils::Stream::ClearPointer(&dest->onKey);
		}

		// Dvar strings
		if (asset->enableDvar)
		{
			buffer->saveString(asset->enableDvar);
			Utils::Stream::ClearPointer(&dest->enableDvar);
		}
		if (asset->localVar)
		{
			buffer->saveString(asset->localVar);
			Utils::Stream::ClearPointer(&dest->localVar);
		}

		// Focus sound
		if (asset->focusSound)
		{
			dest->focusSound = builder->saveSubAsset(Game::XAssetType::ASSET_TYPE_SOUND, asset->focusSound).sound;
		}

		// itemDefData
		if (asset->typeData.data)
		{
			this->save_itemDefData_t(&asset->typeData, asset->type, dest, builder);
		}

		// floatExpressions
		if (asset->floatExpressions)
		{
			buffer->align(Utils::Stream::ALIGN_4);

			Game::ItemFloatExpression* destExp = buffer->dest<Game::ItemFloatExpression>();
			buffer->saveArray(asset->floatExpressions, asset->floatExpressionCount);

			for (int i = 0; i < asset->floatExpressionCount; ++i)
			{
				buffer->align(Utils::Stream::ALIGN_4);
				this->save_Statement_s(asset->floatExpressions[i].expression, builder);
				Utils::Stream::ClearPointer(&destExp->expression);
			}

			Utils::Stream::ClearPointer(&dest->floatExpressions);
		}

		// Statements
		STATEMENT(visibleExp);
		STATEMENT(disabledExp);
		STATEMENT(textExp);
		STATEMENT(materialExp);
	}

	void ImenuDef_t::save(Game::XAssetHeader header, Components::ZoneBuilder::Zone* builder)
	{
		AssertSize(Game::menuDef_t, 400);

		Utils::Stream* buffer = builder->getBuffer();
		Game::menuDef_t* asset = header.menu;
		Game::menuDef_t* dest = buffer->dest<Game::menuDef_t>();

		buffer->save(asset);

		buffer->pushBlock(Game::XFILE_BLOCK_VIRTUAL);

		// ExpressionSupportingData
		if (asset->expressionData)
		{
			// dest->expressionData = nullptr;
			this->save_ExpressionSupportingData(asset->expressionData, builder);
			Utils::Stream::ClearPointer(&dest->expressionData);
		}

		// Window data
		save_windowDef_t<Game::menuDef_t>(&asset->window, dest, builder);

		// Font
		if (asset->font)
		{
			buffer->saveString(asset->font);
			Utils::Stream::ClearPointer(&dest->font);
		}

		// MenuEventHandlerSets
		EVENTHANDLERSET(onOpen);
		EVENTHANDLERSET(onRequestClose);
		EVENTHANDLERSET(onClose);
		EVENTHANDLERSET(onEsc);

		// ItemKeyHandler
		if (asset->onKey)
		{
			buffer->align(Utils::Stream::ALIGN_4);
			this->save_ItemKeyHandler(asset->onKey, builder);
			Utils::Stream::ClearPointer(&dest->onKey);
		}

		// Statement
		STATEMENT(visibleExp);

		// Strings
		if (asset->allowedBinding)
		{
			buffer->saveString(asset->allowedBinding);
			Utils::Stream::ClearPointer(&dest->allowedBinding);
		}
		if (asset->soundLoop)
		{
			buffer->saveString(asset->soundLoop);
			Utils::Stream::ClearPointer(&dest->soundLoop);
		}

		// Statements
		STATEMENT(rectXExp);
		STATEMENT(rectYExp);
		STATEMENT(rectHExp);
		STATEMENT(rectWExp);
		STATEMENT(openSoundExp);
		STATEMENT(closeSoundExp);

		// Items
		if (asset->items)
		{
			buffer->align(Utils::Stream::ALIGN_4);
			buffer->saveArray(asset->items, asset->itemCount);

			for (int i = 0; i < asset->itemCount; ++i)
			{
				if (asset->items[i])
				{
					buffer->align(Utils::Stream::ALIGN_4);
					this->save_itemDef_t(asset->items[i], builder);
				}
			}
		}

		buffer->popBlock();
	}
}
