import { Webhook, RichEmbed } from 'https://deno.land/x/discord_webhook@1.0.0/mod.ts'

export function notify(hook: string, current_documented_functions: number, functions: number) {
	if (current_documented_functions == functions) {
		console.log("All functions documented");
		return;
	} else {
		var embed = new RichEmbed()
		embed.title = "Not all functions documented"
		embed.description = `${current_documented_functions} of ${functions} functions documented, this are ${Math.round(current_documented_functions / functions * 100)}% of the functions`
		embed.color = 0xff0000
		var webhook = new Webhook(hook);
		webhook.post(embed);
	}
}