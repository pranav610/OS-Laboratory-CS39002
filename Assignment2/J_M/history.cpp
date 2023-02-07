#include "history.hpp"

history::history() : max_size(MAX_SIZE)
{
    fp = fopen(HISTORY_FILE, "a");
    if (fp != NULL)
    {
        char *line = NULL;
        size_t len = 0;
        ssize_t read;
        while ((read = getline(&line, &len, fp)) != -1)
        {
            if (get_size() == max_size)
                dequeue.pop_front();
            dequeue.push_back(line);
            free(line);
        }
        fclose(fp);
    }
    curr_ind = dequeue.size();
}

history::~history()
{
    fp = fopen(HISTORY_FILE, "w");
    if (fp != NULL)
    {
        for (int i = 0; i < get_size(); i++)
        {
            fprintf(fp, "%s", dequeue[i].c_str());
        }
        fclose(fp);
    }
}

int history::get_size()
{
    return dequeue.size();
}

bool history::isempty()
{
    return dequeue.empty();
}

void history::add_history(string line)
{
    if (get_size() == max_size)
        dequeue.pop_front();
    dequeue.push_back(line);
    curr_ind = dequeue.size();
}

void history::decrement_history()
{
    if (curr_ind > 0)
        curr_ind--;
}

void history::increment_history()
{
    if (curr_ind < get_size())
        curr_ind++;
}

string history::get_curr()
{
    if (curr_ind == get_size())
        return "";
    return dequeue[curr_ind];
}

history h;
char *curr_line = (char *) NULL;

static int key_up_arrow(int count, int key)
{
    if (count == 0)
        return 0;
    if(h.curr_ind == h.get_size())
        curr_line = strdup(rl_line_buffer);
    h.decrement_history();
    string line = h.get_curr();
    rl_replace_line(line.c_str(), 0);
    rl_point = line.size();
    return 0;
}

static int key_down_arrow(int count, int key)
{
    if (count == 0)
        return 0;
    h.increment_history();
    if (h.curr_ind < h.get_size())
    {
        string line = h.get_curr();
        rl_replace_line(line.c_str(), 0);
        rl_point = line.size();
    }
    else
    {
        rl_replace_line(curr_line, 0);
        rl_point = rl_end;
    }
    return 0;
}

static int key_ctrl_a(int count, int key)
{
    if (count == 0)
        return 0;
    rl_point = 0;
    return 0;
}

static int key_ctrl_e(int count, int key)
{
    if (count == 0)
        return 0;
    rl_point = rl_end;
    return 0;
}

int main()
{
    rl_initialize();
    rl_bind_keyseq("\\e[A", key_up_arrow);
    rl_bind_keyseq("\\e[B", key_down_arrow);
    rl_bind_keyseq("\\C-a", key_ctrl_a);
    rl_bind_keyseq("\\C-e", key_ctrl_e);

    char *input = readline("Enter some text: ");
    while (input != NULL)
    {
        h.add_history(input);
        free(input);
        input = (char *) NULL;
        input = readline("Enter some text: ");
    }
    return 0;
}
