#include "tetris.h"

typedef struct UIData {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *nf;
	Evas_Object *layout;
} UIData;

void create_1player_list(void *data, Evas_Object *obj, void *event_info);
void create_game_start_cb(UIData *ui);

static void
win_delete_request_cb(void *data , Evas_Object *obj , void *event_info)
{
	ui_app_exit();
}

static void
list_selected_cb(void *data, Evas_Object *obj, void *event_info)
{
	Elm_Object_Item *it = event_info;
	elm_list_item_selected_set(it, EINA_FALSE);
}
void // 1인용게임 선택
create_1player_list(void *data, Evas_Object *obj, void *event_info)
{
	UIData *ui = data;
	Evas_Object *list;

	list = elm_list_add(ui->nf);
	elm_list_mode_set(list,ELM_LIST_COMPRESS);
	evas_object_smart_callback_add(list,"selected",list_selected_cb,NULL);

	elm_list_item_append(list,"start",NULL,NULL,create_game_start_cb,ui);

	elm_list_go(list);

	evas_object_show(list);

	elm_naviframe_item_push(ui->nf,"1player",NULL,NULL,list,NULL);

}

static Evas_Object *
create_main_list(UIData *ui)
{
	Evas_Object *list;

	/* List */
	list = elm_list_add(ui->nf);
	elm_list_mode_set(list, ELM_LIST_COMPRESS);
	evas_object_smart_callback_add(list, "selected", list_selected_cb, NULL);

	/* Main Menu Items Here */
	elm_list_item_append(list, "1player", NULL, NULL, create_1player_list, ui);
	elm_list_item_append(list,"2player",NULL,NULL,NULL,ui);
	elm_list_item_append(list,"AI",NULL,NULL,NULL,ui);
	elm_list_item_append(list,"network",NULL,NULL,NULL,ui);
	elm_list_go(list);

	return list;
}

static Eina_Bool
naviframe_pop_cb(void *data, Elm_Object_Item *it)
{
	UIData *ui = data;

	/* Let window go to hide state. */
	elm_win_lower(ui->win);

	return EINA_FALSE;
}
static void
create_base_gui(UIData *ui)
{
	Evas_Object *main_list;
	Elm_Object_Item *nf_it;

	/* Window */
	ui->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
	elm_win_conformant_set(ui->win, EINA_TRUE);
	elm_win_autodel_set(ui->win, EINA_TRUE);

	elm_win_indicator_mode_set(ui->win, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(ui->win, ELM_WIN_INDICATOR_OPAQUE);

	if (elm_win_wm_rotation_supported_get(ui->win)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(ui->win, (const int *)(&rots), 4);
	}

	evas_object_smart_callback_add(ui->win, "delete,request", win_delete_request_cb, NULL);

	/* Conformant */
	ui->conform = elm_conformant_add(ui->win);
	evas_object_size_hint_weight_set(ui->conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ui->win, ui->conform);
	evas_object_show(ui->conform);

	/* Naviframe */
	ui->nf = elm_naviframe_add(ui->conform);
	eext_object_event_callback_add(ui->nf, EEXT_CALLBACK_BACK, eext_naviframe_back_cb, ui);
	evas_object_size_hint_weight_set(ui->nf, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_content_set(ui->conform, ui->nf);
	evas_object_show(ui->nf);

	/* Main list */
	main_list = create_main_list(ui);
	nf_it = elm_naviframe_item_push(ui->nf, "TIZEN TETRIS GAME", NULL, NULL, main_list, NULL);
	elm_naviframe_item_pop_cb_set(nf_it, naviframe_pop_cb, ui);

	/* Show window after base gui is set up */
	evas_object_show(ui->win);
}
static bool app_create(void *user_data)
{
	UIData *ui = user_data;

	elm_app_base_scale_set(1.8);

	create_base_gui(ui);

	return true;
}

static void app_control(app_control_h app_control, void *user_data)
{
	/* Handle the launch request. */
}


static void app_pause(void *user_data)
{
	/* Take necessary actions when application becomes invisible. */
}


static void app_resume(void *user_data)
{
	/* Take necessary actions when application becomes visible. */
}


static void app_terminate(void *user_data)
{
	/* Release all resources. */
}


static void ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LANGUAGE_CHANGED*/
	char *locale = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
	elm_language_set(locale);
	free(locale);
	return;
}


static void ui_app_orient_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_DEVICE_ORIENTATION_CHANGED*/
	return;
}


static void ui_app_region_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_REGION_FORMAT_CHANGED*/
}


static void ui_app_low_battery(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_BATTERY*/
}

static void ui_app_low_memory(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_MEMORY*/
}


int main(int argc, char *argv[])
{
	UIData ui = {0, };
	int ret;

	ui_app_lifecycle_callback_s event_callback = {0, };
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, ui_app_low_battery, &ui);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, ui_app_low_memory, &ui);
	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, ui_app_orient_changed, &ui);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, &ui);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, ui_app_region_changed, &ui);
	ui_app_remove_event_handler(handlers[APP_EVENT_LOW_MEMORY]);

	ret = ui_app_main(argc, argv, &event_callback, &ui);
	if (ret != APP_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "ui_app_main() is failed. err = %d", ret);

	return ret ;
}


