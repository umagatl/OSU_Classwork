/*
 * elevator noop
 */
#include <linux/blkdev.h>
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>

struct noop_data {
	struct list_head queue;
	sector_t last_sector;
	int queue_count;
};

static void noop_print(struct request_queue *q);


/*
 * Redundant code, can just use list_del_init to merge the queue
 */

static void noop_merged_requests(struct request_queue *q, struct request *rq,
				 struct request *next)
{
	list_del_init(&next->queuelist);
}


static int noop_dispatch(struct request_queue *q, int force)
{
	struct noop_data *nd = q->elevator->elevator_data;
	printk("\nIn Dispatch\n");

	if (!list_empty(&nd->queue)) {
		struct request *rq;
		rq = list_entry(nd->queue.next, struct request, queuelist);
		if (rq == 0) {
			printk("Failed to add request to the queue\n");
			return 0;
		}
		
		if (nd->queue_count == 1) {
			printk("Adding single item in queue to dispatch\n");
			list_del_init(&rq->queuelist);
			nd->queue_count--;
		}
		else {
			printk("Adding more than one item in queue to dispatch\n");
			//Get the pointers to the previous and next for comparison
			struct request *curr_req = list_entry(nd->queue.next, struct request, queuelist);
			struct request *prev_req = list_entry(nd->queue.next->prev, struct request, queuelist);
			struct request *next_req = list_entry(nd->queue.next->next, struct request, queuelist);

			//Get the sectors from the pointers
			unsigned long curr_sect = (unsigned long)blk_rq_pos(curr_req);
			unsigned long prev_sect = (unsigned long)blk_rq_pos(prev_req);
			unsigned long next_sect = (unsigned long)blk_rq_pos(next_req);

			//Difference variables
			unsigned long diff_prev = 0;
			unsigned long diff_next = 0;

			if (prev_sect > curr_sect) {
				diff_prev = prev_sect - curr_sect;
			} else if (prev_sect < curr_sect) {
				diff_prev = curr_sect - prev_sect;
			}
			
			if (next_sect > curr_sect) {
				diff_next = next_sect - curr_sect;
			} else if (next_sect < curr_sect) {
				diff_next = curr_sect - next_sect;
			}

			printk("diff_prev: %lu  diff_next: %lu\n", diff_prev, diff_next);
			
			if (diff_prev < diff_next) {
				printk("diff_prev < diff_next. %lu < %lu\n", diff_prev, diff_next);
				nd->queue.next = nd->queue.next->prev;
			}
			else if (diff_prev >= diff_next) {
				printk("diff_next <= diff_prev. %lu <= %lu\n", diff_next, diff_prev);
				nd->queue.next = nd->queue.next->next;
			}
			
			list_del_init(&rq->queuelist);
			nd->queue_count--;
		}
		
		printk("Dispatching :%lu\n", (unsigned long)blk_rq_pos(rq));
		elv_dispatch_sort(q, rq);
		printk("Finished dispatching. Queue_count: %d\n", nd->queue_count);
		noop_print(q);
		printk("\n\n");
		return 1;
	}
	return 0;
}

static void noop_print(struct request_queue *q)
{
	struct noop_data *nd = q->elevator->elevator_data;
	
	struct list_head *entry;
	struct request *item;

	printk("Current queue : ");
	list_for_each(entry, &nd->queue) {
		item = list_entry(entry, struct request, queuelist);
		printk("%lu,", (unsigned long)blk_rq_pos(item));
	}
	printk("\n");
}

static void noop_add_request(struct request_queue *q, struct request *rq)
{
	printk("Adding request: %lu\n", (unsigned long)blk_rq_pos(rq));
	
	struct noop_data *nd = q->elevator->elevator_data;
	int added = 0;

	noop_print(q);

	if (list_empty(&nd->queue)) {
		printk("Empty list, adding single queue\n");
		list_add(&rq->queuelist, &nd->queue);
		nd->queue_count++;
		noop_print(q);
		return;
	}
	
	struct list_head *entry;
	list_for_each(entry, &nd->queue) {
		struct request *curr_req = list_entry(entry, struct request, queuelist);
		struct request *curr_req_next = list_entry(entry->next, struct request, queuelist);

		sector_t curr_req_sect = blk_rq_pos(curr_req);
		sector_t next_req_sect = blk_rq_pos(curr_req_next);
		sector_t new_req_sect = blk_rq_pos(rq);
		
		if (nd->queue_count == 1) {
			added = 1;
			list_add(&rq->queuelist, entry);
			nd->queue_count++;
			printk("Adding a second item. next: %p  prev: %p\n", entry->next, entry->prev);
			break;
		}

		if (next_req_sect >= new_req_sect && curr_req_sect <= new_req_sect) {
			added = 1;
			list_add(&rq->queuelist, entry);
			nd->queue_count++;
			printk("Adding a third or more item. next: %p  prev: %p\n", entry->next, entry->prev);
			break;
		}
	}

	if (added == 0) {	
		list_add_tail(&rq->queuelist, &nd->queue);
		nd->queue_count++;
		printk("Added queue to the tail\n");
	}
		
	printk("Finished adding. Queue_count: %d\n\n", nd->queue_count);
	noop_print(q);
}


static struct request *
noop_former_request(struct request_queue *q, struct request *rq)
{
	struct noop_data *nd = q->elevator->elevator_data;

	if (rq->queuelist.prev == &nd->queue)
		return NULL;
	return list_entry(rq->queuelist.prev, struct request, queuelist);
}

static struct request *
noop_latter_request(struct request_queue *q, struct request *rq)
{
	struct noop_data *nd = q->elevator->elevator_data;

	if (rq->queuelist.next == &nd->queue)
		return NULL;
	return list_entry(rq->queuelist.next, struct request, queuelist);
}


static int noop_init_queue(struct request_queue *q, struct elevator_type *e)
{
	struct noop_data *nd;
	struct elevator_queue *eq;

	eq = elevator_alloc(q, e);
	if (!eq)
		return -ENOMEM;
	
	nd = kmalloc_node(sizeof(*nd), GFP_KERNEL, q->node);
	if (!nd) {
		kobject_put(&eq->kobj);
		return -ENOMEM;
		//return NULL;
	}
	eq->elevator_data = nd;

	INIT_LIST_HEAD(&nd->queue);

	spin_lock_irq(q->queue_lock);
	q->elevator = eq;
	spin_unlock_irq(q->queue_lock);

	nd->queue_count = 0;
	
	//return nd;
	return 0;
}

static void noop_exit_queue(struct elevator_queue *e)
{
	struct noop_data *nd = e->elevator_data;

	BUG_ON(!list_empty(&nd->queue));
	kfree(nd);
}

static int noop_deny_merge(struct request_queue *req_q, struct request *req, struct bio *bio)
{
	return ELEVATOR_NO_MERGE;
}

static struct elevator_type elevator_noop = {
	.ops = {
		.elevator_merge_req_fn		= noop_merged_requests,
		.elevator_allow_merge_fn 	= noop_deny_merge,
		.elevator_dispatch_fn		= noop_dispatch,
		.elevator_add_req_fn		= noop_add_request,
		.elevator_former_req_fn		= noop_former_request,
		.elevator_latter_req_fn		= noop_latter_request,
		.elevator_init_fn		= noop_init_queue,
		.elevator_exit_fn		= noop_exit_queue,
	},
	.elevator_name = "sstf",
	.elevator_owner = THIS_MODULE,
};

static int __init noop_init(void)
{
	return elv_register(&elevator_noop);
}

static void __exit noop_exit(void)
{
	elv_unregister(&elevator_noop);
}

module_init(noop_init);
module_exit(noop_exit);


MODULE_AUTHOR("Lance Umagat");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("SSTF IO scheduler");
