#include "sigs.h"
#include <gtk/gtk.h>

pid_t* pid_list;
int my_floor_number,current_floor_number;
GtkWidget *window;
GtkWidget *call_btn;
GtkWidget *close_btn;
GtkWidget *button_box;
GtkWidget *main_box;
GtkWidget *up_down_btn;
GtkWidget *current_floor_btn;
char window_title[10],current_floor[10],current_label[5];
GdkPixbuf *create_pixbuf(const gchar * filename) {
    
   GdkPixbuf *pixbuf;
   GError *error = NULL;
   pixbuf = gdk_pixbuf_new_from_file(filename, &error);
   
   if (!pixbuf) {
       
      fprintf(stderr, "%s\n", error->message);
      g_error_free(error);
   }

   return pixbuf;
}

void current_floor_change(int sigNo){

    // current_floor_number=sigNo-SIGRTMIN-F1_ARRIVAL+1;
    // if(my_floor_number==3) printf("%d va %d\n",current_floor_number,sigNo-SIGRTMIN-F1_ARRIVAL+1 );
    // if(current_floor_number>sigNo-SIGRTMIN-F1_ARRIVAL+1){
    //     gtk_widget_set_name(current_floor_btn, "unready_btn");        
    // } 
    // current_floor_number=sigNo-SIGRTMIN-F1_ARRIVAL+1;           
    switch(sigNo-SIGRTMIN){
        case F1_ARRIVAL:            
            gtk_button_set_label(GTK_BUTTON(current_floor_btn),"1");            
            break;
        case F2_ARRIVAL:            
            gtk_button_set_label(GTK_BUTTON(current_floor_btn),"2");
            break;
        case F3_ARRIVAL:            
            gtk_button_set_label(GTK_BUTTON(current_floor_btn),"3");
            break;
        case F4_ARRIVAL:            
            gtk_button_set_label(GTK_BUTTON(current_floor_btn),"4");
            break;
        case F5_ARRIVAL:            
            gtk_button_set_label(GTK_BUTTON(current_floor_btn),"5");
            break;  
        default:            
            break;
    }
    gtk_widget_set_name(current_floor_btn, "unready_btn");   
}
void finish_move(){    
    //printf("Tang %d get arrives notification\n",my_floor_number);      
    gtk_widget_set_name(call_btn, "default_btn");
}
void using(){
    gtk_button_set_label(GTK_BUTTON(current_floor_btn),"");
    gtk_widget_set_name(current_floor_btn, "ready_btn");
}
void finish_move_and_using(){    
    //printf("Tang %d get arrives notification\n",my_floor_number);
    gtk_button_set_label(GTK_BUTTON(current_floor_btn),"");
    gtk_widget_set_name(current_floor_btn, "ready_btn");    
    gtk_widget_set_name(call_btn, "default_btn");
}
static void
call_func(GtkWidget *widget,
      gpointer data)
{   // Set id cho button de su dung css #red_btn
    signal(SIGRTMIN+F1_ARRIVAL,SIG_IGN);signal(SIGRTMIN+F2_ARRIVAL,SIG_IGN);signal(SIGRTMIN+F3_ARRIVAL,SIG_IGN);signal(SIGRTMIN+F4_ARRIVAL,SIG_IGN);signal(SIGRTMIN+F5_ARRIVAL,SIG_IGN);    
    signal(SIGRTMIN+FINISHED,SIG_IGN);
    signal(SIGRTMIN+USING,SIG_IGN);
    signal(SIGRTMIN+FINISHEDUSING,SIG_IGN);
    if(strcmp(gtk_widget_get_name(call_btn),"red_btn")!=0){
        gtk_widget_set_name(call_btn, "red_btn");       
        //g_print("Ban da click call_btn at floor: %d\n",this_floor-SIGRTMIN-10);
        send_signal(pid_list[LIFT_MNG],(my_floor_number-1)+SIGRTMIN+F1_CALL);    
    }
    signal(SIGRTMIN+F1_ARRIVAL,current_floor_change);signal(SIGRTMIN+F2_ARRIVAL,current_floor_change);signal(SIGRTMIN+F3_ARRIVAL,current_floor_change);signal(SIGRTMIN+F4_ARRIVAL,current_floor_change);signal(SIGRTMIN+F5_ARRIVAL,current_floor_change);    
    signal(SIGRTMIN+FINISHED,finish_move);
    signal(SIGRTMIN+USING,using);
    signal(SIGRTMIN+FINISHEDUSING,finish_move_and_using);
}
static void
activate(GtkApplication *app,
         gpointer user_data)
{
    // <-------------  For add stylesheet.css
    GtkCssProvider* Provider = gtk_css_provider_new();
    GdkDisplay* Display = gdk_display_get_default();
    GdkScreen* Screen = gdk_display_get_default_screen(Display);

    gtk_style_context_add_provider_for_screen(Screen, GTK_STYLE_PROVIDER(Provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_css_provider_load_from_path(GTK_CSS_PROVIDER(Provider), "stylesheet.css", NULL);
    // End add stylesheet.css ---------------->
    
    //printf("%d\n",my_floor_number);
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), window_title);
    gtk_window_set_default_size(GTK_WINDOW(window), 150, 100);
        
    gtk_window_set_icon(GTK_WINDOW(window), create_pixbuf("icon"));
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
    //   Add Vbox
    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), main_box);

    button_box = gtk_button_box_new(GTK_ORIENTATION_VERTICAL);
    gtk_container_add(GTK_CONTAINER(main_box), button_box);

    switch(my_floor_number){
        case 5:
            gtk_window_move(GTK_WINDOW(window),50,50);
            up_down_btn = gtk_button_new_with_label("FLOOR 5");
        break;
        case 4:
            up_down_btn = gtk_button_new_with_label("FLOOR 4");
            gtk_window_move(GTK_WINDOW(window),50,160);
        break;
        case 3:
            up_down_btn = gtk_button_new_with_label("FLOOR 3");
            gtk_window_move(GTK_WINDOW(window),50,270);
        break;
        case 2:
            up_down_btn = gtk_button_new_with_label("FLOOR 2");
            gtk_window_move(GTK_WINDOW(window),50,380);
        break;
        default:break;
    }
    gtk_widget_set_name(up_down_btn, "info_btn");
    gtk_widget_set_sensitive(up_down_btn, FALSE);
    gtk_container_add(GTK_CONTAINER(button_box), up_down_btn);

    current_floor_btn = gtk_button_new_with_label("1");
    gtk_widget_set_name(current_floor_btn, "unready_btn");

    gtk_widget_set_sensitive(current_floor_btn, FALSE);
    gtk_container_add(GTK_CONTAINER(button_box), current_floor_btn);
    // button x
    call_btn = gtk_button_new_with_label("CALL");
    g_signal_connect(call_btn, "clicked", G_CALLBACK(call_func),NULL);
    gtk_container_add(GTK_CONTAINER(button_box), call_btn);
    
    // show all widget
    gtk_widget_show_all(window);
}
int main(int argc, char *argv[])
{	
	GtkApplication *app;
    int status;
    signal(SIGRTMIN+F1_ARRIVAL,current_floor_change);signal(SIGRTMIN+F2_ARRIVAL,current_floor_change);signal(SIGRTMIN+F3_ARRIVAL,current_floor_change);signal(SIGRTMIN+F4_ARRIVAL,current_floor_change);signal(SIGRTMIN+F5_ARRIVAL,current_floor_change);    
	signal(SIGRTMIN+FINISHED,finish_move);
    signal(SIGRTMIN+USING,using);
    signal(SIGRTMIN+FINISHEDUSING,finish_move_and_using);
    if(argc!=2){
		printf("Usage: opx FLOOR_NUMBER\n"); exit(0);
	}		
	int this_floor;
    my_floor_number=atoi(argv[1]);
    printf("ope%d_process_id %d\n",my_floor_number, getpid() );
	switch(my_floor_number){
		case 2:
        
		pid_list=update_pid(OPE_PANE2);        
        setpgid(pid_list[OPE_PANE2],pid_list[LIFT_MNG]);
		this_floor=SIGRTMIN+F2_CALL;
		strcpy(window_title,"Tầng 2");
		app = gtk_application_new("org.gtk.tang2", G_APPLICATION_FLAGS_NONE);
		break;
		case 3:
        
		pid_list=update_pid(OPE_PANE3);
        setpgid(pid_list[OPE_PANE3],pid_list[LIFT_MNG]);
		this_floor=SIGRTMIN+F3_CALL;
		strcpy(window_title,"Tầng 3");
		app = gtk_application_new("org.gtk.tang3", G_APPLICATION_FLAGS_NONE);
		break;
		case 4:
        
		pid_list=update_pid(OPE_PANE4);
        setpgid(pid_list[OPE_PANE4],pid_list[LIFT_MNG]);
		this_floor=SIGRTMIN+F4_CALL;
		strcpy(window_title,"Tầng 4");
		app = gtk_application_new("org.gtk.tang4", G_APPLICATION_FLAGS_NONE);
		break;
		case 5:
        
		pid_list=update_pid(OPE_PANE5);
        setpgid(pid_list[OPE_PANE5],pid_list[LIFT_MNG]); 
		this_floor=SIGRTMIN+F5_CALL;
		strcpy(window_title,"Tầng 5");
		app = gtk_application_new("org.gtk.tang5", G_APPLICATION_FLAGS_NONE);
		break;
		default:break;
	}	
    g_signal_connect(app, "activate", G_CALLBACK(activate), &this_floor);
    status = g_application_run(G_APPLICATION(app), 0, NULL);
    g_object_unref(app);
    return status;    
}